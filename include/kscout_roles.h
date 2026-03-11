/*
 * kscout_role.h
 *
 * Enumeration of all Football Manager player roles and duties, grouped by
 * positional area. Each valid role+duty combination gets its own enum value
 * so it can be used directly as an array index for weight tables.
 *
 * Naming convention:  KSCOUT_ROLE_<ROLE>_<DUTY>
 * Duty suffixes:      _D  = Defend
 *                     _S  = Support
 *                     _A  = Attack
 *                     _ST = Stopper   (central defender variant)
 *                     _CO = Cover     (central defender variant)
 *
 * Source: FM20 positions / roles / duties map.
 */

#ifndef KSCOUT_ROLE_H
#define KSCOUT_ROLE_H

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_ROLE_RATING_INITIAL_CAP 8

typedef struct kscout_role_rating_s kscout_role_rating_t;
typedef struct kscout_role_score_s kscout_role_score_t;
typedef struct kscout_role_weights_s kscout_role_weights_t;

/* -------------------------------------------------------------------------
 * Player attributes enumeration
 *
 * Each value is the index into kscout_player_t.attributes[].
 * German abbreviations preserved to match FM export column headers exactly.
 * ---------------------------------------------------------------------- */

typedef enum {
  KSCOUT_ATTR_1V1 = 0,   /* 1v1  (GK: one-on-ones)      */
  KSCOUT_ATTR_ACC = 1,   /* Acc  (Acceleration)          */
  KSCOUT_ATTR_AER = 2,   /* Aer  (Aerial Reach / GK)     */
  KSCOUT_ATTR_AGG = 3,   /* Agg  (Aggression)            */
  KSCOUT_ATTR_AGI = 4,   /* Agi  (Agility)               */
  KSCOUT_ATTR_ANT = 5,   /* Ant  (Anticipation)          */
  KSCOUT_ATTR_BAL = 6,   /* Bal  (Balance)               */
  KSCOUT_ATTR_BRA = 7,   /* Bra  (Bravery)               */
  KSCOUT_ATTR_CMD = 8,   /* Cmd  (Command of Area / GK)  */
  KSCOUT_ATTR_CNT = 9,   /* Cnt  (Concentration)         */
  KSCOUT_ATTR_COM = 10,  /* Com  (Communication / GK)    */
  KSCOUT_ATTR_CMP = 11,  /* Cmp  (Composure)             */
  KSCOUT_ATTR_COR = 12,  /* Cor  (Corners)               */
  KSCOUT_ATTR_CRO = 13,  /* Cro  (Crossing)              */
  KSCOUT_ATTR_DEC = 14,  /* Dec  (Decisions)             */
  KSCOUT_ATTR_DET = 15,  /* Det  (Determination)         */
  KSCOUT_ATTR_ECC = 16,  /* Ecc  (Eccentricity / GK)     */
  KSCOUT_ATTR_DRI = 17,  /* Dri  (Dribbling)             */
  KSCOUT_ATTR_FIN = 18,  /* Fin  (Finishing)             */
  KSCOUT_ATTR_FIR = 19,  /* Fir  (First Touch)           */
  KSCOUT_ATTR_FRE = 20,  /* Fre  (Free Kick Taking)      */
  KSCOUT_ATTR_FLA = 21,  /* Fla  (Flair)                 */
  KSCOUT_ATTR_HAN = 22,  /* Han  (Handling / GK)         */
  KSCOUT_ATTR_HEA = 23,  /* Hea  (Heading)               */
  KSCOUT_ATTR_JUM = 24,  /* Jum  (Jumping Reach)         */
  KSCOUT_ATTR_KIC = 25,  /* Kic  (Kicking / GK)          */
  KSCOUT_ATTR_LDR = 26,  /* Ldr  (Leadership)            */
  KSCOUT_ATTR_LON = 27,  /* Lon  (Long Shots)            */
  KSCOUT_ATTR_L_TH = 28, /* L Th (Long Throw)            */
  KSCOUT_ATTR_MAR = 29,  /* Mar  (Marking)               */
  KSCOUT_ATTR_NAT = 30,  /* Nat  (Natural Fitness)       */
  KSCOUT_ATTR_OTB = 31,  /* OtB  (Off the Ball)          */
  KSCOUT_ATTR_PAC = 32,  /* Pac  (Pace)                  */
  KSCOUT_ATTR_PAS = 33,  /* Pas  (Passing)               */
  KSCOUT_ATTR_POS = 34,  /* Pos  (Positioning)           */
  KSCOUT_ATTR_REF = 35,  /* Ref  (Reflexes / GK)         */
  KSCOUT_ATTR_STA = 36,  /* Sta  (Stamina)               */
  KSCOUT_ATTR_STR = 37,  /* Str  (Strength)              */
  KSCOUT_ATTR_TCK = 38,  /* Tck  (Tackling)              */
  KSCOUT_ATTR_TEA = 39,  /* Tea  (Teamwork)              */
  KSCOUT_ATTR_TEC = 40,  /* Tec  (Technique)             */
  KSCOUT_ATTR_THR = 41,  /* Thr  (Throwing / GK)         */
  KSCOUT_ATTR_PEN = 42,  /* Pen  (Penalty Taking)        */
  KSCOUT_ATTR_PUN = 43,  /* Pun  (Punching / GK)         */
  KSCOUT_ATTR_TRO = 44,  /* TRO  (Rushing Out / GK)      */
  KSCOUT_ATTR_VIS = 45,  /* Vis  (Vision)                */
  KSCOUT_ATTR_WOR = 46,  /* Wor  (Work Rate)             */

  KSCOUT_ATTR_COUNT = 47 /* total number of attributes                 */
} kscout_attr_t;

typedef enum {
  KSCOUT_CAT_TECHNICAL = 0,
  KSCOUT_CAT_MENTAL = 1,
  KSCOUT_CAT_PHYSICAL = 2,
  KSCOUT_CAT_GK = 3,
  KSCOUT_CAT_COUNT = 4,
} kscout_attr_cat_t;

// Player attributes ordered by category e.g. technical, mental, physical,
// goalkeeper
//  Each row is terminated by -1. Second dimension = largest category (14) +
//  sentinel.
static const int kscout_attr_by_cat[KSCOUT_CAT_COUNT][15] = {
    [KSCOUT_CAT_TECHNICAL] = {KSCOUT_ATTR_COR, KSCOUT_ATTR_CRO, KSCOUT_ATTR_DRI,
                              KSCOUT_ATTR_FIN, KSCOUT_ATTR_FIR, KSCOUT_ATTR_FRE,
                              KSCOUT_ATTR_HEA, KSCOUT_ATTR_LON,
                              KSCOUT_ATTR_L_TH, KSCOUT_ATTR_MAR,
                              KSCOUT_ATTR_PAS, KSCOUT_ATTR_PEN, KSCOUT_ATTR_TCK,
                              KSCOUT_ATTR_TEC, -1},
    [KSCOUT_CAT_MENTAL] = {KSCOUT_ATTR_AGG, KSCOUT_ATTR_ANT, KSCOUT_ATTR_BRA,
                           KSCOUT_ATTR_CNT, KSCOUT_ATTR_CMP, KSCOUT_ATTR_DEC,
                           KSCOUT_ATTR_DET, KSCOUT_ATTR_FLA, KSCOUT_ATTR_LDR,
                           KSCOUT_ATTR_OTB, KSCOUT_ATTR_POS, KSCOUT_ATTR_TEA,
                           KSCOUT_ATTR_VIS, KSCOUT_ATTR_WOR, -1},
    [KSCOUT_CAT_PHYSICAL] = {KSCOUT_ATTR_ACC, KSCOUT_ATTR_AGI, KSCOUT_ATTR_BAL,
                             KSCOUT_ATTR_JUM, KSCOUT_ATTR_NAT, KSCOUT_ATTR_PAC,
                             KSCOUT_ATTR_STA, KSCOUT_ATTR_STR, -1},
    [KSCOUT_CAT_GK] = {KSCOUT_ATTR_1V1, KSCOUT_ATTR_AER, KSCOUT_ATTR_CMD,
                       KSCOUT_ATTR_COM, KSCOUT_ATTR_ECC, KSCOUT_ATTR_HAN,
                       KSCOUT_ATTR_KIC, KSCOUT_ATTR_REF, KSCOUT_ATTR_THR,
                       KSCOUT_ATTR_PUN, KSCOUT_ATTR_TRO, -1},
};

struct kscout_role_score_s {
  const kscout_role_weights_t *def;
  float score;
};

struct kscout_role_rating_s {
  kscout_role_score_t *items;
  size_t count;
  size_t capacity;
};

struct kscout_role_weights_s {
  const char *name;
  uint32_t valid_positions; // currently omitted
  int attribute_weights[KSCOUT_ATTR_COUNT];
  int max_weighted_score;
};

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_ROLE_H */