#ifndef KSCOUT_PLAYER_H
#define KSCOUT_PLAYER_H

#include <stdint.h>
#include "kscout_roles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_MAX_PLAYER_FOOT 2
#define KSCOUT_PLAYER_FOOT_R 0
#define KSCOUT_PLAYER_FOOT_L 1

#define KSCOUT_MAX_ATTR_VALUE 20

typedef struct kscout_player_s kscout_player_t;
typedef struct kscout_transfer_value_s kscout_transfer_value_t;
typedef int kscout_position_t;

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
  GK = 0, // Goalkeeper
  LD,     // Left Defender
  CDL,    // Central Defender (Left)
  CDC,    // Central Defender (Center)
  CDR,    // Central Defender (Right)
  RD,     // Right Defender
  WBL,    // Wing Back (Left)
  DML,    // Defensive Midfielder (Left)
  DMC,    // Defensive Midfielder (Center)
  DMR,    // Defensive Midfielder (Right)
  WBR,    // Wing Back (Right)
  WL,     // Wide Midfielder (Left)
  ML,     // Midfielder (Left)
  MC,     // Midfielder (Center)
  MR,     // Midfielder (Right)
  WR,     // Wide Midfielder (Right)
  AWL,    // Attacking Wide Midfielder (Left)
  AML,    // Attacking Midfielder (Left)
  AMC,    // Attacking Midfielder (Center)
  AMR,    // Attacking Midfielder (Right)
  AWR,    // Attacking Wide Midfielder (Right)
  FWDL,   // Forward (Left)
  FWDC,   // Forward (Center)
  FWDR    // Forward (Right)
} kscout_position_flags_t;

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
  char* info;
  char* position2;
  char* position;
  int attributes[KSCOUT_ATTR_COUNT];
  kscout_role_rating_t role_rating;
};

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_PLAYER_H
