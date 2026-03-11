#ifndef KSCOUT_PLAYER_H
#define KSCOUT_PLAYER_H

#include "kscout_roles.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_MAX_PLAYER_FOOT 2
#define KSCOUT_PLAYER_FOOT_R 0
#define KSCOUT_PLAYER_FOOT_L 1

#define KSCOUT_MAX_ATTR_VALUE 20

typedef struct kscout_player_s kscout_player_t;
typedef struct kscout_transfer_value_s kscout_transfer_value_t;

/* -------------------------------------------------------------------------
 * Transfer value range (in whole euros)
 * ---------------------------------------------------------------------- */

struct kscout_transfer_value_s {
  uint32_t lo;
  uint32_t hi;
};

/**
 * Footedness:
 * 1 - 4:very weak,
 * 5- 7: weak,
 * 8 - 11: reasonable,
 * 12-14: fairly strong,
 * 15 -17: strong
 * 18 - 20 very strong
 */
typedef enum {
  KSCOUT_FOOT_VERY_WEAK = 1,
  KSCOUT_FOOT_WEAK = 2,
  KSCOUT_FOOT_REASONABLE = 3,
  KSCOUT_FOOT_FAIRLY_STRONG = 4,
  KSCOUT_FOOT_STRONG = 5,
  KSCOUT_FOOT_VERY_STRONG = 6,
} kscout_footedness_t;

typedef enum {
  KSCOUT_POS_GK = 0, // Goalkeeper
  KSCOUT_POS_FBL,    // Left Defender
  KSCOUT_POS_CDL,    // Central Defender (Left)
  KSCOUT_POS_CDC,    // Central Defender (Center)
  KSCOUT_POS_CDR,    // Central Defender (Right)
  KSCOUT_POS_FBR,    // Right Defender
  KSCOUT_POS_WBL,    // Wing Back (Left)
  KSCOUT_POS_DML,    // Defensive Midfielder (Left)
  KSCOUT_POS_DMC,    // Defensive Midfielder (Center)
  KSCOUT_POS_DMR,    // Defensive Midfielder (Right)
  KSCOUT_POS_WBR,    // Wing Back (Right)
  KSCOUT_POS_WL,     // Wide Midfielder (Left)
  KSCOUT_POS_ML,     // Midfielder (Left)
  KSCOUT_POS_MC,     // Midfielder (Center)
  KSCOUT_POS_MR,     // Midfielder (Right)
  KSCOUT_POS_WR,     // Wide Midfielder (Right)
  KSCOUT_POS_AWL,    // Attacking Wide Midfielder (Left)
  KSCOUT_POS_AML,    // Attacking Midfielder (Left)
  KSCOUT_POS_AMC,    // Attacking Midfielder (Center)
  KSCOUT_POS_AMR,    // Attacking Midfielder (Right)
  KSCOUT_POS_AWR,    // Attacking Wide Midfielder (Right)
  KSCOUT_POS_FWDL,   // Forward (Left)
  KSCOUT_POS_FWDC,   // Forward (Center)
  KSCOUT_POS_FWDR    // Forward (Right)
} kscout_position_t;

struct kscout_player_s {
  unsigned uid;
  char *name;
  int age;
  uint16_t height_cm;
  uint16_t weight_kg;
  char *nationality;
  char *nationality2;
  char *club;
  char *league;
  uint16_t goals;
  uint32_t appearances;
  uint32_t wage;
  kscout_transfer_value_t transfer_value;
  kscout_footedness_t foot_left;
  kscout_footedness_t foot_right;
  char *pro;
  char *contra;
  char *personality;
  char *media_handling;
  char *info;
  uint32_t positions;         /* bitmask of all positions, bit N = (1u <<
                                 kscout_position_t) */
  uint32_t positions_natural; /* bitmask of natural / best position */
  int attributes[KSCOUT_ATTR_COUNT];
  kscout_role_rating_t role_rating;
};

/* -------------------------------------------------------------------------
 * Position lookup table (FM German abbreviations -> kscout_position_t)
 * ---------------------------------------------------------------------- */

typedef struct {
  const char *token;
  kscout_position_t pos;
} kscout_pos_entry_t;

static const kscout_pos_entry_t kscout_pos_map[] = {
  {"TW", KSCOUT_POS_GK},       {"LA", KSCOUT_POS_FBL},
  {"IV", KSCOUT_POS_CDC},      {"RA", KSCOUT_POS_FBR},
  {"LV", KSCOUT_POS_WBL},      {"RV", KSCOUT_POS_WBR},
  {"DM", KSCOUT_POS_DMC},      {"DM (L)", KSCOUT_POS_DML},
  {"DM (R)", KSCOUT_POS_DMR},  {"M (L)", KSCOUT_POS_ML},
  {"M (Z)", KSCOUT_POS_MC},    {"M (R)", KSCOUT_POS_MR},
    {"OM (L)", KSCOUT_POS_AML},  {"OM (Z)", KSCOUT_POS_AMC},
    {"OM (R)", KSCOUT_POS_AMR},  {"FV (L)", KSCOUT_POS_AWL},
    {"FV (R)", KSCOUT_POS_AWR},  {"V (L)", KSCOUT_POS_FWDL},
    {"V (Z)", KSCOUT_POS_FWDC},  {"V (R)", KSCOUT_POS_FWDR},
    {"ST (Z)", KSCOUT_POS_FWDC},
};

#define KSCOUT_POS_MAP_LEN (sizeof(kscout_pos_map) / sizeof(kscout_pos_map[0]))

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_PLAYER_H
