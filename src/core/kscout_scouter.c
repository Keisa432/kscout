#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define STB_DS_IMPLEMENTATION
#include "../../libs/stb/stb_ds.h"

#include "kscout_da.h"
#include "kscout_memblock.h"
#include "kscout_roles.h"
#include "kscout_scouter.h"
#include "kscout_utils.h"

/* --------------------------------------------------------------------------
 * Internal Types
 * -------------------------------------------------------------------------- */

typedef struct kscout_role_entry_s kscout_role_entry_t;

struct kscout_role_entry_s {
  char *key;
  kscout_role_weights_t value;
};

struct kscout_scouter_s {
  kscout_role_entry_t *roles;
  kscout_memblock_t strs;
};

/* --------------------------------------------------------------------------
 * Internal helpers
 * -------------------------------------------------------------------------- */

static int kscout_scoring_load_role_configs(kscout_scouter_t *scouter,
                                            const char *cfg_path)
{
  DIR *dir = opendir(cfg_path);
  if (!dir) {
    return KSCOUT_ERR_IO;
  }

  struct dirent *ent;
  char path[PATH_MAX];

  while ((ent = readdir(dir)) != NULL) {
    struct stat st;

    snprintf(path, sizeof(path), "%s/%s", cfg_path, ent->d_name);

    if (stat(path, &st) != 0) {
      continue;
    }

    if (!S_ISREG(st.st_mode)) {
      continue;
    }

    if (!has_suffix(ent->d_name, ".krs")) {
      continue;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
      continue; /* or return error if strict */
    }

    char line[256];
    const char *role_key = NULL;
    while (fgets(line, sizeof(line), fp)) {
      char *p = line;

      if (*p == '\0' || *p == '#')
        continue;

      char *eq = strchr(p, '=');
      if (!eq)
        continue;

      *eq = '\0';
      char _key_buf[256];
      char _val_buf[256];
      const char *key = trim(p, _key_buf, 256);
      const char *val = trim((eq + 1), _val_buf, 256);

      /* skip metadata */
      if (strcmp(key, "type") == 0)
        continue;

      if (strcmp(key, "positions") == 0) {
        if (role_key != NULL) {
          kscout_role_weights_t *rw =
              &shgetp(scouter->roles, role_key)->value;
          char pos_buf[256];
          strncpy(pos_buf, val, sizeof(pos_buf) - 1);
          pos_buf[sizeof(pos_buf) - 1] = '\0';
          char *tok = pos_buf;
          while (tok) {
            char *sep = strstr(tok, ", ");
            if (sep)
              *sep = '\0';
            for (size_t i = 0; i < KSCOUT_POS_MAP_LEN; i++) {
              if (strcmp(tok, kscout_pos_map[i].token) == 0)
                KSCOUT_POS_SET(rw->valid_positions, kscout_pos_map[i].pos);
            }
            tok = sep ? sep + 2 : NULL;
          }
        }
        continue;
      }

      if (strcmp(key, "name") == 0) {
        role_key = kscout_memblock_strdup(&scouter->strs, val);
        if (shgeti(scouter->roles, role_key) >= 0) {
          // abort parsing of file -> role entries must be unique
          break;
        }

        kscout_role_weights_t zero = {0};
        zero.name = role_key;
        shput(scouter->roles, role_key, zero);
        continue;
      }

      /* attribute weight */
      if (role_key != NULL) {
        kscout_attr_t attr = kscout_scouter_attr_by_key(key);
        if (attr != KSCOUT_ATTR_COUNT) {
          char *end;
          unsigned long w = strtoul(val, &end, 10);
          if (*end == '\0') {
            kscout_role_weights_t *rw =
                &shgetp(scouter->roles, role_key)->value;
            rw->attribute_weights[attr] = (int)w;
            rw->max_weighted_score +=
                KSCOUT_MAX_ATTR_VALUE * rw->attribute_weights[attr];
          }
        }
      }
    }

    fclose(fp);
  }

  closedir(dir);
  return KSCOUT_OK;
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int kscout_scouter_new(kscout_scouter_t **scouter, const char *cfg_path)
{
  int rc = KSCOUT_OK;

  if (!scouter || !cfg_path) {
    return KSCOUT_ERR_INVALID;
  }

  kscout_scouter_t *s = calloc(1, sizeof(*s));
  if (!s) {
    return KSCOUT_ERR_OOM;
  }

  if (kscout_memblock_init(&s->strs, 4096) != 0) {
    rc = KSCOUT_ERR_OOM;
    goto l_error;
  }

  rc = kscout_scoring_load_role_configs(s, cfg_path);
  if (rc != KSCOUT_OK) {
    goto l_error;
  }

  *scouter = s;

  return rc;

l_error:
  shfree(s->roles);
  kscout_memblock_destroy(&s->strs);
  free(s);
  return rc;
}

void kscout_scouter_destory(kscout_scouter_t *scouter)
{
  if (scouter) {
    shfree(scouter->roles);
    kscout_memblock_destroy(&scouter->strs);
    free(scouter);
  }
}

int kscout_scouter_report_create(kscout_scouter_t *scouter,
                                 kscout_report_t *report)
{
  if (!scouter || !report) {
    return KSCOUT_ERR_INVALID;
  }

  /* Normalized attribute ratings per category (0..20) */
  for (int cat = 0; cat < KSCOUT_CAT_COUNT; cat++) {
    int sum = 0, count = 0;
    for (int j = 0; j < 15 && kscout_attr_by_cat[cat][j] != -1; j++) {
      sum += report->player.attributes[kscout_attr_by_cat[cat][j]];
      count++;
    }
    report->attr_rating[cat] =
        count > 0 ? (float)sum / ((float)count * KSCOUT_MAX_ATTR_VALUE) *
                        KSCOUT_MAX_ATTR_VALUE
                  : 0.0f;
  }

  kscout_role_rating_t *rating = &report->role_rating;
  for (int i = 0; i < shlen(scouter->roles); i++) {
    const kscout_role_weights_t *rw = &scouter->roles[i].value;
    int weighted_role_score = 0;
    for (int j = 0; j < KSCOUT_ATTR_COUNT; j++) {
      weighted_role_score +=
          report->player.attributes[j] * rw->attribute_weights[j];
    }

    float score = ((float)weighted_role_score / rw->max_weighted_score) *
                  KSCOUT_MAX_ATTR_VALUE;

    kscout_role_score_t entry = {
        .def = rw,
        .score = score,
    };

    if (entry.score > report->best_overall_role.score) {
      report->best_overall_role = entry;
    }

    if (KSCOUT_POS_MATCH(report->player.positions_natural,
                         entry.def->valid_positions) &&
        entry.score > report->best_position_role.score) {
      report->best_position_role = entry;
    }

    kscout_da_push(rating, entry);
  }

  return KSCOUT_OK;
}