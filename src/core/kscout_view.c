#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "kscout_da.h"
#include "kscout_parser.h"
#include "kscout_player_creator.h"
#include "kscout_scouter.h"
#include "kscout_view.h"

typedef struct kscout_view_reports_s kscout_view_reports_t;

struct kscout_view_reports_s {
  kscout_report_t *items;
  size_t capacity;
  size_t count;
};

struct kscout_view_s {
  kscout_parser_t *parser;
  kscout_player_creator_t *creator;
  kscout_scouter_t *scouter;
  kscout_memblock_t block;
  kscout_view_reports_t player_reports;
};

struct kscout_view_iter_s {
  kscout_view_t *view;
  size_t index;
};

/* --------------------------------------------------------------------------
 * Internal helpers
 * -------------------------------------------------------------------------- */

/**
 * \brief callback function to crate a player form parsed tokens. it is then
 * added to the data base
 *
 * \param tokens parsed tokens
 * \param count number of parsed tokens
 * \param user callback agrument (kscout_view_t*)
 */
static void kscout_view_player_from_tokens_cb(kscout_parser_token_t *tokens,
                                              unsigned count, void *user)
{
  kscout_view_t *view = (kscout_view_t *)user;
  kscout_report_t new_report = {0};

  if (!view) {
    return;
  }

  if (kscout_player_creator_player_from_tokens(view->creator, tokens, count,
                                               &new_report.player,
                                               &view->block) == 0) {
    kscout_scouter_report_create(view->scouter, &new_report);
    kscout_da_push(&view->player_reports, new_report);
  }
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

/**
 * \brief Create new data base instance
 *
 * \param db data base handle
 * \return 0 on success, -1 on error
 */
int kscout_view_new(kscout_view_t **db, kscout_scouter_t *scouter)
{
  if (!db || !scouter) {
    return KSCOUT_ERR_INVALID;
  }

  int rc = KSCOUT_OK;
  kscout_view_t *ctx;

  do {
    ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
      rc = KSCOUT_ERR_OOM;
      break;
    }

    rc = kscout_memblock_init(&ctx->block, 2048);
    if (rc != KSCOUT_OK) {
      break;
    }

    ctx->scouter = scouter;
  } while (0);

  if (rc != KSCOUT_OK) {
    kscout_memblock_destroy(&ctx->block);
    free(ctx);
    return rc;
  }

  *db = ctx;

  return KSCOUT_OK;
}

/**
 * \brief Destory database
 *
 * \param db database handle
 */
void kscout_view_destroy(kscout_view_t *view)
{
  kscout_da_foreach(kscout_report_t, report, &view->player_reports)
  {
    kscout_da_free(report->role_rating);
  }
  kscout_da_free(view->player_reports);
  free(view);
}

/**
 * \brief load data base from file
 *
 * \param db data base handle
 * \param path path to shortlist file in rtf format
 * \return 0 on success,-1 on allocation or configuration error
 */
int kscout_view_load_file(kscout_view_t **db, const char *path)
{
  int rc = 0;
  kscout_view_t *ctx;

  if (!path || strlen(path) == 0) {
    return -1;
  }

  do {
    kscout_parser_cfg_t cfg = {
        .type = KSCOUT_PARSER_RTF,
        .cb = kscout_view_player_from_tokens_cb,
        .cb_arg = ctx,
    };
    kscout_parser_t *parser = NULL;

    rc = kscout_parser_new(&ctx->parser, &cfg);
    if (rc != 0) {
      break;
    }

    rc = kscout_player_creator_new(&ctx->creator);
    if (rc != 0) {
      break;
    }

    rc = kscout_parser_import(ctx->parser, path);

    kscout_parser_destroy(ctx->parser);
    ctx->parser = NULL;

    kscout_player_creator_destroy(ctx->creator);
    ctx->creator = NULL;
  } while (0);

  if (rc != 0) {
    kscout_view_destroy(ctx);
    return rc;
  }

  *db = ctx;

  return 0;
}

int kscout_view_export_to_json(kscout_view_t *view, const char *file_path)
{
  if (!view || !file_path) {
    return KSCOUT_ERR_INVALID;
  }

  int rc = KSCOUT_OK;
  char *json_str = NULL;
  FILE *f = NULL;

  cJSON *root = cJSON_CreateObject();
  if (!root) {
    return KSCOUT_ERR_OOM;
  }

  cJSON *players_array = cJSON_AddArrayToObject(root, "players");
  if (!players_array) {
    rc = KSCOUT_ERR_OOM;
    goto cleanup;
  }

  kscout_da_foreach(kscout_report_t, report, &view->player_reports)
  {
    cJSON *player_obj = cJSON_CreateObject();
    if (!player_obj) {
      rc = KSCOUT_ERR_OOM;
      goto cleanup;
    }
    cJSON_AddItemToArray(players_array, player_obj);

    cJSON_AddNumberToObject(player_obj, "uid", report->player.uid);
    cJSON_AddStringToObject(player_obj, "name",
                            report->player.name ? report->player.name : "");

    cJSON *roles_array = cJSON_AddArrayToObject(player_obj, "roles");
    if (!roles_array) {
      rc = KSCOUT_ERR_OOM;
      goto cleanup;
    }

    kscout_da_foreach(kscout_role_score_t, rs, &report->role_rating)
    {
      if (!rs->def)
        continue;
      cJSON *role_obj = cJSON_CreateObject();
      if (!role_obj) {
        rc = KSCOUT_ERR_OOM;
        goto cleanup;
      }
      cJSON_AddItemToArray(roles_array, role_obj);
      cJSON_AddStringToObject(role_obj, "role", rs->def->name);
      char _fbuf[32] = {0};
      snprintf(_fbuf, sizeof(_fbuf), "%.2f", rs->score);
      cJSON_AddRawToObject(role_obj, "score", _fbuf);
    }
  }

  json_str = cJSON_Print(root);
  if (!json_str) {
    rc = KSCOUT_ERR_OOM;
    goto cleanup;
  }

  f = fopen(file_path, "w");
  if (!f) {
    rc = KSCOUT_ERR_IO;
    goto cleanup;
  }
  fputs(json_str, f);
  fclose(f);

cleanup:
  free(json_str);
  cJSON_Delete(root);
  return rc;
}

int kscout_view_export_to_csv(kscout_view_t *view, const char *file_path)
{
  if (!view || !file_path) {
    return KSCOUT_ERR_INVALID;
  }

  if (view->player_reports.count == 0) {
    return KSCOUT_OK;
  }

  FILE *f = fopen(file_path, "w");
  if (!f) {
    return KSCOUT_ERR_IO;
  }

  kscout_report_t *first = &view->player_reports.items[0];

  /* header */
  fprintf(f, "UID;Name;Technical;Mental;Physical");
  kscout_da_foreach(kscout_role_score_t, rs, &first->role_rating)
  {
    if (rs->def) {
      fprintf(f, ";%s", rs->def->name);
    }
  }
  fputc('\n', f);

  /* rows */
  kscout_da_foreach(kscout_report_t, report, &view->player_reports)
  {
    fprintf(f, "%u;%s;%.2f;%.2f;%.2f", report->player.uid,
            report->player.name ? report->player.name : "",
            report->attr_rating[KSCOUT_CAT_TECHNICAL],
            report->attr_rating[KSCOUT_CAT_MENTAL],
            report->attr_rating[KSCOUT_CAT_PHYSICAL]);

    size_t col = 0;
    kscout_da_foreach(kscout_role_score_t, hdr, &first->role_rating)
    {
      if (!hdr->def) {
        col++;
        continue;
      }

      float score = 0.0f;
      if (col < report->role_rating.count) {
        kscout_role_score_t *rs = &report->role_rating.items[col];
        if (rs->def && strcmp(rs->def->name, hdr->def->name) == 0) {
          score = rs->score;
        } else {
          /* order mismatch: fall back to linear search */
          for (size_t i = 0; i < report->role_rating.count; i++) {
            kscout_role_score_t *s = &report->role_rating.items[i];
            if (s->def && strcmp(s->def->name, hdr->def->name) == 0) {
              score = s->score;
              break;
            }
          }
        }
      }

      fprintf(f, ";%.2f", score);
      col++;
    }

    fputc('\n', f);
  }

  fclose(f);
  return KSCOUT_OK;
}

int kscout_view_iter_init(kscout_view_t *view, kscout_view_iter_t **iter)
{
  kscout_view_iter_t *new_it = NULL;
  if (!view || !iter) {
    return -1;
  }

  new_it = calloc(1, sizeof(*new_it));

  new_it->view = view;

  *iter = new_it;

  return 0;
}

int kscout_view_iter_next(kscout_view_iter_t *iter, kscout_report_t *report)
{

  if (iter->index >= iter->view->player_reports.count) {
    return KSCOUT_ERR_OUT_OF_BOUNDS;
  }

  *report = iter->view->player_reports.items[iter->index];
  iter->index++;
  return KSCOUT_OK;
}

void kscout_view_iter_destroy(kscout_view_iter_t *iter) { free(iter); }
