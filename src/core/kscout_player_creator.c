/*
 * kscout_player_creator.c
 *
 * Maps a fixed-order token array (as exported from FM) to a kscout_player_t.
 * Token order is positional — no key lookup required.
 *
 * Column order (0-based index):
 *   0  EID            1  Name           2  Position       3  2. Position
 *   4  Alter          5  Größe          6  Gewicht        7  Info
 *   8  Verein         9  Liga           10 Nation         11 2. Nation
 *   12 Lsp            13 Tore           14 Persönlichkeit 15 Medienumgang
 *   16 Gehalt         17 Transferwert   18 Linker Fuß     19 Rechter Fuß
 *   20 Pro            21 Kontra
 *   22+ attributes (see kscout_player_attr_t)
 */
#include "kscout_player_creator.h"
#include "kscout_memblock.h"
#include "kscout_parser.h"
#include "kscout_player.h"
#include "kscout_utils.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





static void pos_lookup(const char *key, uint32_t *mask)
{
  for (size_t i = 0; i < KSCOUT_POS_MAP_LEN; i++) {
    if (strcmp(key, kscout_pos_map[i].token) == 0)
      *mask |= (1u << kscout_pos_map[i].pos);
  }
}

/*
 * Parse an FM position string into a bitmask.
 *
 * Grammar:
 *   positions = group { ", " group }
 *   group     = codes [ " (" laterals ")" ]
 *   codes     = code { "/" code }
 *   laterals  = one or more of 'R', 'L', 'Z'
 *
 * Examples: "TW", "M (RLZ)", "V/FV (RL)", "OM (R), ST (Z)",
 *           "V/FV/M/OM (R)", "DM, M (Z)"
 */
static uint32_t positions_from_str(const char *str)
{
  uint32_t mask = 0;
  if (!str || !*str || strcmp(str, "-") == 0)
    return 0;

  char buf[128];
  strncpy(buf, str, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';

  char *p = buf;
  while (p) {
    /* split on literal ", " */
    char *sep = strstr(p, ", ");
    if (sep)
      *sep = '\0';

    /* extract optional " (RLZ)" lateral suffix */
    char lat[8] = "";
    char *lp = strrchr(p, '(');
    if (lp && lp > p && *(lp - 1) == ' ') {
      size_t li = 0;
      for (char *c = lp + 1; *c && *c != ')' && li < sizeof(lat) - 1; c++)
        lat[li++] = *c;
      lat[li] = '\0';
      *(lp - 1) = '\0';
    }

    /* split codes by '/' and expand each lateral letter */
    char *code = strtok(p, "/");
    while (code) {
      if (lat[0]) {
        for (const char *l = lat; *l; l++) {
          char key[16];
          snprintf(key, sizeof(key), "%s (%c)", code, *l);
          pos_lookup(key, &mask);
        }
      } else {
        pos_lookup(code, &mask);
      }
      code = strtok(NULL, "/");
    }

    p = sep ? sep + 2 : NULL;
  }
  return mask;
}

static int parse_positions(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[128];
  *(uint32_t *)dest = positions_from_str(TRIM(str));
  return 1;
}

/* -------------------------------------------------------------------------
 * Parse functions
 * signature: int parse_xxx(const char *str, void *dest, kscout_memblock_t *mb)
 * returns 1 on success, 0 on failure / not applicable
 * ---------------------------------------------------------------------- */

static int parse_string(const char *str, void *dest, kscout_memblock_t *mb)
{
  char _tbuf[512];
  const char *s = TRIM(str);
  if (strcmp(s, "-") == 0) {
    s = "";
  }

  *(char **)dest = kscout_memblock_strdup(mb, s);
  return 1;
}

static int parse_uint32(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[64];
  const char *s = TRIM(str);
  char *end;
  errno = 0;
  unsigned long v = strtoul(s, &end, 10);
  if (end == s || errno)
    return 0;
  *(uint32_t *)dest = (uint32_t)v;
  return 1;
}

static int parse_uint8(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[32];
  const char *s = TRIM(str);
  char *end;
  errno = 0;
  unsigned long v = strtoul(s, &end, 10);
  if (end == s || errno)
    return 0;
  *(uint8_t *)dest = (uint8_t)v;
  return 1;
}

/* Parse "177 cm" -> 177 */
static int parse_height(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[32];
  const char *s = TRIM(str);
  char *end;
  errno = 0;
  unsigned long v = strtoul(s, &end, 10);
  if (end == s || errno)
    return 0;
  *(uint16_t *)dest = (uint16_t)v;
  return 1;
}

/* Parse "75 kg" -> 75 */
static int parse_weight(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[32];
  const char *s = TRIM(str);
  char *end;
  errno = 0;
  unsigned long v = strtoul(s, &end, 10);
  if (end == s || errno)
    return 0;
  *(uint16_t *)dest = (uint16_t)v;
  return 1;
}

/*
 * Parse wage strings like "€6,750/W." or "6,750/W." -> raw integer (cents or
 * whole currency unit, your choice — here we store the integer part only).
 * Returns 0 if unparseable (e.g. "-").
 */
static int parse_wage(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[64];
  const char *s = TRIM(str);
  if (strcmp(s, "-") == 0) {
    *(uint32_t *)dest = 0;
    return 1;
  }
  /* skip currency symbol */
  while (*s && (*s < '0' || *s > '9'))
    s++;
  char buf[32];
  size_t i = 0;
  while (*s && i < sizeof(buf) - 1) {
    if (*s >= '0' && *s <= '9')
      buf[i++] = *s;
    else if (*s == '.')
      break; /* e.g. "1.1Mio" handled below */
    else if (*s == ',') {
      s++;
      continue;
    } /* thousands separator */
    else
      break;
    s++;
  }
  buf[i] = '\0';
  if (i == 0) {
    *(uint32_t *)dest = 0;
    return 1;
  }
  *(uint32_t *)dest = (uint32_t)strtoul(buf, NULL, 10);
  return 1;
}

/*
 * Parse transfer value ranges like "€180K - €500K" or "€1.1Mio".
 * Stores the low and high end in kscout_transfer_value_t {uint32_t lo, hi;} in
 * units of whole euros (K/Mio expanded).
 */
static uint32_t parse_value_token(const char *s)
{
  while (*s && (*s < '0' || *s > '9'))
    s++;
  char buf[32];
  size_t i = 0;
  int is_decimal = 0;
  char dec_buf[8] = {0};
  size_t di = 0;
  while (*s && i < sizeof(buf) - 1) {
    if (*s >= '0' && *s <= '9') {
      if (is_decimal) {
        if (di < sizeof(dec_buf) - 1)
          dec_buf[di++] = *s;
      } else
        buf[i++] = *s;
    } else if (*s == '.') {
      is_decimal = 1;
    } else if (*s == ',') {
      s++;
      continue;
    } else
      break;
    s++;
  }
  buf[i] = '\0';
  uint32_t base = (uint32_t)strtoul(buf, NULL, 10);
  /* check suffix */
  while (*s == ' ')
    s++;
  uint32_t multiplier = 1;
  if (s[0] == 'K' || s[0] == 'k')
    multiplier = 1000;
  else if ((s[0] == 'M' || s[0] == 'm'))
    multiplier = 1000000;
  uint32_t result = base * multiplier;
  if (is_decimal && dec_buf[0]) {
    uint32_t frac = (uint32_t)strtoul(dec_buf, NULL, 10);
    /* e.g. 1.1Mio: frac digit represents tenths */
    result += (frac * multiplier) / 10;
  }
  return result;
}

static int parse_transfer_value(const char *str, void *dest,
                                kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[64];
  const char *s = TRIM(str);
  kscout_transfer_value_t *tv = (kscout_transfer_value_t *)dest;
  tv->lo = 0;
  tv->hi = 0;
  if (strcmp(s, "-") == 0)
    return 1;
  /* find separator " - " */
  const char *sep = strstr(s, " - ");
  if (sep) {
    char lo_buf[32];
    size_t len = (size_t)(sep - s);
    if (len >= sizeof(lo_buf))
      len = sizeof(lo_buf) - 1;
    memcpy(lo_buf, s, len);
    lo_buf[len] = '\0';
    tv->lo = parse_value_token(lo_buf);
    tv->hi = parse_value_token(sep + 3);
  } else {
    tv->lo = tv->hi = parse_value_token(s);
  }
  return 1;
}

/* Parse foot rating strings: "Sehr stark"->6, "Stark"->5, "Gut" ->4
 * "Passabel"->3, "Schwach"->2, "Sehr schwach"->1, else 0 */
static int parse_foot(const char *str, void *dest, kscout_memblock_t *mb)
{
  (void)mb;
  char _tbuf[32];
  const char *s = TRIM(str);
  kscout_footedness_t v = KSCOUT_FOOT_VERY_WEAK;
  if (strncmp(s, "Sehr stark", 10) == 0) {
    v = KSCOUT_FOOT_VERY_STRONG;
  } else if (strncmp(s, "Stark", 5) == 0) {
    v = KSCOUT_FOOT_STRONG;
  } else if (strncmp(s, "Gut", 3) == 0) {
    v = KSCOUT_FOOT_FAIRLY_STRONG;
  } else if (strncmp(s, "Passabel", 8) == 0) {
    v = KSCOUT_FOOT_REASONABLE;
  } else if (strncmp(s, "Schwach", 7) == 0) {
    v = KSCOUT_FOOT_WEAK;
  } else if (strncmp(s, "Sehr schwach", 12) == 0) {
    v = KSCOUT_FOOT_VERY_WEAK;
  }
  *(kscout_footedness_t *)dest = v;
  return 1;
}

/* -------------------------------------------------------------------------
 * Field map — positional, index == column index in token array
 * ---------------------------------------------------------------------- */

typedef int (*kscout_parse_token_fn)(const char *str, void *dest,
                                     kscout_memblock_t *mb);

typedef struct {
  size_t offset;  /* offsetof into kscout_player_t; 0 if attr */
  int attr_index; /* -1 if not an attribute */
  kscout_parse_token_fn parse;
} kscout_field_t;

/* Convenience macros */
#define FIELD(member, fn) {offsetof(kscout_player_t, member), -1, fn}
#define ATTR(idx) {0, (idx), parse_uint8}
#define SKIP {0, -1, NULL}

static const kscout_field_t kscout_fields[] = {
    /* 0  */ FIELD(uid, parse_uint32),
    /* 1  */ FIELD(name, parse_string),
    /* 2  */ FIELD(positions, parse_positions),
    /* 3  */ FIELD(positions_natural, parse_positions),
    /* 4  */ FIELD(age, parse_uint8),
    /* 5  */ FIELD(height_cm, parse_height),
    /* 6  */ FIELD(weight_kg, parse_weight),
    /* 7  */ FIELD(info, parse_string),
    /* 8  */ FIELD(club, parse_string),
    /* 9  */ FIELD(league, parse_string),
    /* 10 */ FIELD(nationality, parse_string),
    /* 11 */ FIELD(nationality2, parse_string),
    /* 12 */ FIELD(appearances, parse_uint32),
    /* 13 */ FIELD(goals, parse_uint32),
    /* 14 */ FIELD(personality, parse_string),
    /* 15 */ FIELD(media_handling, parse_string),
    /* 16 */ FIELD(wage, parse_wage),
    /* 17 */ FIELD(transfer_value, parse_transfer_value),
    /* 18 */ FIELD(foot_left, parse_foot),
    /* 19 */ FIELD(foot_right, parse_foot),
    /* 20 */ FIELD(pro, parse_string),
    /* 21 */ FIELD(contra, parse_string),
    /* attributes start at index 22 */
    /* 22 */ ATTR(KSCOUT_ATTR_1V1),
    /* 23 */ ATTR(KSCOUT_ATTR_ACC),
    /* 24 */ ATTR(KSCOUT_ATTR_AER),
    /* 25 */ ATTR(KSCOUT_ATTR_AGG),
    /* 26 */ ATTR(KSCOUT_ATTR_AGI),
    /* 27 */ ATTR(KSCOUT_ATTR_ANT),
    /* 28 */ ATTR(KSCOUT_ATTR_BAL),
    /* 29 */ ATTR(KSCOUT_ATTR_BRA),
    /* 30 */ ATTR(KSCOUT_ATTR_CMD),
    /* 31 */ ATTR(KSCOUT_ATTR_CNT),
    /* 32 */ ATTR(KSCOUT_ATTR_COM),
    /* 33 */ ATTR(KSCOUT_ATTR_CMP),
    /* 34 */ ATTR(KSCOUT_ATTR_COR),
    /* 35 */ ATTR(KSCOUT_ATTR_CRO),
    /* 36 */ ATTR(KSCOUT_ATTR_DEC),
    /* 37 */ ATTR(KSCOUT_ATTR_DET),
    /* 38 */ ATTR(KSCOUT_ATTR_ECC),
    /* 39 */ ATTR(KSCOUT_ATTR_DRI),
    /* 40 */ ATTR(KSCOUT_ATTR_FIN),
    /* 41 */ ATTR(KSCOUT_ATTR_FIR),
    /* 42 */ ATTR(KSCOUT_ATTR_FRE),
    /* 43 */ ATTR(KSCOUT_ATTR_FLA),
    /* 44 */ ATTR(KSCOUT_ATTR_HAN),
    /* 45 */ ATTR(KSCOUT_ATTR_HEA),
    /* 46 */ ATTR(KSCOUT_ATTR_JUM),
    /* 47 */ ATTR(KSCOUT_ATTR_KIC),
    /* 48 */ ATTR(KSCOUT_ATTR_LDR),
    /* 49 */ ATTR(KSCOUT_ATTR_LON),
    /* 50 */ ATTR(KSCOUT_ATTR_L_TH),
    /* 51 */ ATTR(KSCOUT_ATTR_MAR),
    /* 52 */ ATTR(KSCOUT_ATTR_NAT),
    /* 53 */ ATTR(KSCOUT_ATTR_OTB),
    /* 54 */ ATTR(KSCOUT_ATTR_PAC),
    /* 55 */ ATTR(KSCOUT_ATTR_PAS),
    /* 56 */ ATTR(KSCOUT_ATTR_POS),
    /* 57 */ ATTR(KSCOUT_ATTR_REF),
    /* 58 */ ATTR(KSCOUT_ATTR_STA),
    /* 59 */ ATTR(KSCOUT_ATTR_STR),
    /* 60 */ ATTR(KSCOUT_ATTR_TCK),
    /* 61 */ ATTR(KSCOUT_ATTR_TEA),
    /* 62 */ ATTR(KSCOUT_ATTR_TEC),
    /* 63 */ ATTR(KSCOUT_ATTR_THR),
    /* 64 */ ATTR(KSCOUT_ATTR_PEN),
    /* 65 */ ATTR(KSCOUT_ATTR_PUN),
    /* 66 */ ATTR(KSCOUT_ATTR_TRO),
    /* 67 */ ATTR(KSCOUT_ATTR_VIS),
    /* 68 */ ATTR(KSCOUT_ATTR_WOR),
};

#define KSCOUT_NUM_FIELDS (sizeof(kscout_fields) / sizeof(kscout_fields[0]))

/* -------------------------------------------------------------------------
 * Creator object
 * ---------------------------------------------------------------------- */

struct kscout_player_creator {
  int dummy; /* reserved for future config (locale, strict mode, …) */
};

int kscout_player_creator_new(kscout_player_creator_t **c)
{
  kscout_player_creator_t *new = calloc(1, sizeof(*c));
  if (!c) {
    return -1;
  }

  *c = new;

  return 0;
}

void kscout_player_creator_destroy(kscout_player_creator_t *c) { free(c); }

/* -------------------------------------------------------------------------
 * Main conversion
 * ---------------------------------------------------------------------- */

int kscout_player_creator_player_from_tokens(
    const kscout_player_creator_t *creator, const kscout_parser_token_t *tokens,
    size_t num_tokens, kscout_player_t *player, kscout_memblock_t *mb)
{
  if (!creator || !tokens || !player)
    return -1;

  memset(player, 0, sizeof(*player));

  size_t limit =
      num_tokens < KSCOUT_NUM_FIELDS ? num_tokens : KSCOUT_NUM_FIELDS;

  for (size_t i = 0; i < limit; i++) {
    const kscout_field_t *f = &kscout_fields[i];
    if (!f->parse) {
      continue; /* SKIP */
    }

    const char *token = tokens[i].value ? tokens[i].value : "";

    if (f->attr_index >= 0) {
      /* attribute path */
      f->parse(token, &player->attributes[f->attr_index], mb);
    } else {
      /* struct member path */
      f->parse(token, (char *)player + f->offset, mb);
    }
  }

  return 0;
}