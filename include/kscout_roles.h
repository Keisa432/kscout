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

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_ROLE_RATING_INITIAL_CAP 8

typedef struct kscout_role_rating_s kscout_role_rating_t;
typedef struct kscout_role_score_s kscout_role_score_t;
typedef struct kscout_role_weights_s kscout_role_weights_t;

typedef enum {

    /* ------------------------------------------------------------------
     * Goalkeeper
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_GOALKEEPER_D,                   /* GK  — Defend          */
    KSCOUT_ROLE_SWEEPER_KEEPER_D,               /* SK  — Defend          */
    KSCOUT_ROLE_SWEEPER_KEEPER_S,               /* SK  — Support         */
    KSCOUT_ROLE_SWEEPER_KEEPER_A,               /* SK  — Attack          */

    /* ------------------------------------------------------------------
     * Central Defence
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_CENTRAL_DEFENDER_D,             /* CD  — Defend          */
    KSCOUT_ROLE_CENTRAL_DEFENDER_ST,            /* CD  — Stopper         */
    KSCOUT_ROLE_CENTRAL_DEFENDER_CO,            /* CD  — Cover           */
    KSCOUT_ROLE_BALL_PLAYING_DEFENDER_D,        /* BPD — Defend          */
    KSCOUT_ROLE_BALL_PLAYING_DEFENDER_ST,       /* BPD — Stopper         */
    KSCOUT_ROLE_BALL_PLAYING_DEFENDER_CO,       /* BPD — Cover           */
    KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_D,      /* NCB — Defend          */
    KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_ST,     /* NCB — Stopper         */
    KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_CO,     /* NCB — Cover           */
    KSCOUT_ROLE_LIBERO_S,                       /* LIB — Support         */
    KSCOUT_ROLE_LIBERO_A,                       /* LIB — Attack          */

    /* ------------------------------------------------------------------
     * Full Back
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_FULL_BACK_D,                    /* FB  — Defend          */
    KSCOUT_ROLE_FULL_BACK_S,                    /* FB  — Support         */
    KSCOUT_ROLE_FULL_BACK_A,                    /* FB  — Attack          */
    KSCOUT_ROLE_DEFENSIVE_WING_BACK_D,          /* DWB — Defend          */

    /* ------------------------------------------------------------------
     * Wing Back
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_WING_BACK_D,                    /* WB  — Defend          */
    KSCOUT_ROLE_WING_BACK_S,                    /* WB  — Support         */
    KSCOUT_ROLE_WING_BACK_A,                    /* WB  — Attack          */
    KSCOUT_ROLE_COMPLETE_WING_BACK_S,           /* CWB — Support         */
    KSCOUT_ROLE_COMPLETE_WING_BACK_A,           /* CWB — Attack          */
    KSCOUT_ROLE_INVERTED_WING_BACK_D,           /* IWB — Defend          */
    KSCOUT_ROLE_INVERTED_WING_BACK_S,           /* IWB — Support         */
    KSCOUT_ROLE_INVERTED_WING_BACK_A,           /* IWB — Attack          */

    /* ------------------------------------------------------------------
     * Defensive Midfield
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_ANCHOR_MAN_D,                   /* AM  — Defend          */
    KSCOUT_ROLE_HALF_BACK_D,                    /* HB  — Defend          */
    KSCOUT_ROLE_BALL_WINNING_MIDFIELDER_D,      /* BWM — Defend          */
    KSCOUT_ROLE_BALL_WINNING_MIDFIELDER_S,      /* BWM — Support         */
    KSCOUT_ROLE_DEFENSIVE_MIDFIELDER_D,         /* DM  — Defend          */
    KSCOUT_ROLE_DEFENSIVE_MIDFIELDER_S,         /* DM  — Support         */
    KSCOUT_ROLE_DEEP_LYING_PLAYMAKER_D,         /* DLP — Defend          */
    KSCOUT_ROLE_DEEP_LYING_PLAYMAKER_S,         /* DLP — Support         */
    KSCOUT_ROLE_SEGUNDO_VOLANTE_S,              /* SV  — Support         */
    KSCOUT_ROLE_SEGUNDO_VOLANTE_A,              /* SV  — Attack          */

    /* ------------------------------------------------------------------
     * Central Midfield
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_CENTRAL_MIDFIELDER_D,           /* CM  — Defend          */
    KSCOUT_ROLE_CENTRAL_MIDFIELDER_S,           /* CM  — Support         */
    KSCOUT_ROLE_CENTRAL_MIDFIELDER_A,           /* CM  — Attack          */
    KSCOUT_ROLE_BOX_TO_BOX_MIDFIELDER_S,        /* BBM — Support         */
    KSCOUT_ROLE_CARRILERO_S,                    /* CAR — Support         */
    KSCOUT_ROLE_MEZZALA_S,                      /* MEZ — Support         */
    KSCOUT_ROLE_MEZZALA_A,                      /* MEZ — Attack          */
    KSCOUT_ROLE_ROAMING_PLAYMAKER_S,            /* RPM — Support         */

    /* ------------------------------------------------------------------
     * Attacking Midfield (central)
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_ADVANCED_PLAYMAKER_S,           /* AP  — Support         */
    KSCOUT_ROLE_ADVANCED_PLAYMAKER_A,           /* AP  — Attack          */
    KSCOUT_ROLE_ATTACKING_MIDFIELDER_S,         /* AM  — Support         */
    KSCOUT_ROLE_ATTACKING_MIDFIELDER_A,         /* AM  — Attack          */
    KSCOUT_ROLE_ENGANCHE_S,                     /* ENG — Support         */
    KSCOUT_ROLE_SHADOW_STRIKER_A,               /* SS  — Attack          */
    KSCOUT_ROLE_TREQUARTISTA_A,                 /* TQ  — Attack (AMC)    */

    /* ------------------------------------------------------------------
     * Wide Midfield
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_WIDE_MIDFIELDER_D,              /* WM  — Defend          */
    KSCOUT_ROLE_WIDE_MIDFIELDER_S,              /* WM  — Support         */
    KSCOUT_ROLE_WIDE_MIDFIELDER_A,              /* WM  — Attack          */
    KSCOUT_ROLE_WIDE_PLAYMAKER_S,               /* WP  — Support         */
    KSCOUT_ROLE_WIDE_PLAYMAKER_A,               /* WP  — Attack          */

    /* ------------------------------------------------------------------
     * Wingers / Wide Attackers
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_WINGER_S,                       /* W   — Support         */
    KSCOUT_ROLE_WINGER_A,                       /* W   — Attack          */
    KSCOUT_ROLE_INSIDE_FORWARD_S,               /* IF  — Support         */
    KSCOUT_ROLE_INSIDE_FORWARD_A,               /* IF  — Attack          */
    KSCOUT_ROLE_INVERTED_WINGER_S,              /* IW  — Support         */
    KSCOUT_ROLE_INVERTED_WINGER_A,              /* IW  — Attack          */
    KSCOUT_ROLE_RAUMDEUTER_A,                   /* RMD — Attack          */
    KSCOUT_ROLE_WIDE_TARGET_MAN_S,              /* WTM — Support         */
    KSCOUT_ROLE_WIDE_TARGET_MAN_A,              /* WTM — Attack          */

    /* ------------------------------------------------------------------
     * Striker
     * ------------------------------------------------------------------ */
    KSCOUT_ROLE_ADVANCED_FORWARD_A,             /* AF  — Attack          */
    KSCOUT_ROLE_COMPLETE_FORWARD_S,             /* CF  — Support         */
    KSCOUT_ROLE_COMPLETE_FORWARD_A,             /* CF  — Attack          */
    KSCOUT_ROLE_DEEP_LYING_FORWARD_S,           /* DLF — Support         */
    KSCOUT_ROLE_DEEP_LYING_FORWARD_A,           /* DLF — Attack          */
    KSCOUT_ROLE_FALSE_NINE_S,                   /* F9  — Support         */
    KSCOUT_ROLE_POACHER_A,                      /* PO  — Attack          */
    KSCOUT_ROLE_PRESSING_FORWARD_D,             /* PF  — Defend          */
    KSCOUT_ROLE_PRESSING_FORWARD_S,             /* PF  — Support         */
    KSCOUT_ROLE_PRESSING_FORWARD_A,             /* PF  — Attack          */
    KSCOUT_ROLE_TARGET_MAN_S,                   /* TM  — Support         */
    KSCOUT_ROLE_TARGET_MAN_A,                   /* TM  — Attack          */
    KSCOUT_ROLE_TREQUARTISTA_ST_A,              /* TQ  — Attack (ST)     */

    /* Sentinel */
    KSCOUT_ROLE_COUNT

} kscout_role_t;

/* -------------------------------------------------------------------------
* Player attributes enumeration
*
* Each value is the index into kscout_player_t.attributes[].
* German abbreviations preserved to match FM export column headers exactly.
* ---------------------------------------------------------------------- */

typedef enum {
  KSCOUT_ATTR_1V1 = 0,         /* 1v1  (GK: one-on-ones)      */
  KSCOUT_ATTR_ACC = 1,         /* Acc  (Acceleration)          */
  KSCOUT_ATTR_AER = 2,         /* Aer  (Aerial Reach / GK)     */
  KSCOUT_ATTR_AGG = 3,         /* Agg  (Aggression)            */
  KSCOUT_ATTR_AGI = 4,         /* Agi  (Agility)               */
  KSCOUT_ATTR_ANT = 5,         /* Ant  (Anticipation)          */
  KSCOUT_ATTR_BAL = 6,         /* Bal  (Balance)               */
  KSCOUT_ATTR_BRA = 7,         /* Bra  (Bravery)               */
  KSCOUT_ATTR_CMD = 8,         /* Cmd  (Command of Area / GK)  */
  KSCOUT_ATTR_CNT = 9,         /* Cnt  (Concentration)         */
  KSCOUT_ATTR_COM = 10,        /* Com  (Communication / GK)    */
  KSCOUT_ATTR_CMP = 11,        /* Cmp  (Composure)             */
  KSCOUT_ATTR_COR = 12,        /* Cor  (Corners)               */
  KSCOUT_ATTR_CRO = 13,        /* Cro  (Crossing)              */
  KSCOUT_ATTR_DEC = 14,        /* Dec  (Decisions)             */
  KSCOUT_ATTR_DET = 15,        /* Det  (Determination)         */
  KSCOUT_ATTR_ECC = 16,        /* Ecc  (Eccentricity / GK)     */
  KSCOUT_ATTR_DRI = 17,        /* Dri  (Dribbling)             */
  KSCOUT_ATTR_FIN = 18,        /* Fin  (Finishing)             */
  KSCOUT_ATTR_FIR = 19,        /* Fir  (First Touch)           */
  KSCOUT_ATTR_FRE = 20,        /* Fre  (Free Kick Taking)      */
  KSCOUT_ATTR_FLA = 21,        /* Fla  (Flair)                 */
  KSCOUT_ATTR_HAN = 22,        /* Han  (Handling / GK)         */
  KSCOUT_ATTR_HEA = 23,        /* Hea  (Heading)               */
  KSCOUT_ATTR_JUM = 24,        /* Jum  (Jumping Reach)         */
  KSCOUT_ATTR_KIC = 25,        /* Kic  (Kicking / GK)          */
  KSCOUT_ATTR_LDR = 26,        /* Ldr  (Leadership)            */
  KSCOUT_ATTR_LON = 27,        /* Lon  (Long Shots)            */
  KSCOUT_ATTR_L_TH = 28,       /* L Th (Long Throw)            */
  KSCOUT_ATTR_MAR = 29,        /* Mar  (Marking)               */
  KSCOUT_ATTR_NAT = 30,        /* Nat  (Natural Fitness)       */
  KSCOUT_ATTR_OTB = 31,        /* OtB  (Off the Ball)          */
  KSCOUT_ATTR_PAC = 32,        /* Pac  (Pace)                  */
  KSCOUT_ATTR_PAS = 33,        /* Pas  (Passing)               */
  KSCOUT_ATTR_POS = 34,        /* Pos  (Positioning)           */
  KSCOUT_ATTR_REF = 35,        /* Ref  (Reflexes / GK)         */
  KSCOUT_ATTR_STA = 36,        /* Sta  (Stamina)               */
  KSCOUT_ATTR_STR = 37,        /* Str  (Strength)              */
  KSCOUT_ATTR_TCK = 38,        /* Tck  (Tackling)              */
  KSCOUT_ATTR_TEA = 39,        /* Tea  (Teamwork)              */
  KSCOUT_ATTR_TEC = 40,        /* Tec  (Technique)             */
  KSCOUT_ATTR_THR = 41,        /* Thr  (Throwing / GK)         */
  KSCOUT_ATTR_PEN = 42,        /* Pen  (Penalty Taking)        */
  KSCOUT_ATTR_PUN = 43,        /* Pun  (Punching / GK)         */
  KSCOUT_ATTR_TRO = 44,        /* TRO  (Rushing Out / GK)      */
  KSCOUT_ATTR_VIS = 45,        /* Vis  (Vision)                */
  KSCOUT_ATTR_WOR = 46,        /* Wor  (Work Rate)             */

  KSCOUT_ATTR_COUNT = 47 /* total number of attributes                 */
} kscout_attr_t;

/* -------------------------------------------------------------------------
 * String key table — index maps to kscout_role_t.
 * Format: "<role>-<duty>" e.g. "box-to-box-midfielder-s"
 * Used as the value of the 'name' field in a role config file.
 * ---------------------------------------------------------------------- */

static const char *kscout_role_keys[] = {
    [KSCOUT_ROLE_GOALKEEPER_D]                = "goalkeeper-d",
    [KSCOUT_ROLE_SWEEPER_KEEPER_D]            = "sweeper-keeper-d",
    [KSCOUT_ROLE_SWEEPER_KEEPER_S]            = "sweeper-keeper-s",
    [KSCOUT_ROLE_SWEEPER_KEEPER_A]            = "sweeper-keeper-a",

    [KSCOUT_ROLE_CENTRAL_DEFENDER_D]          = "central-defender-d",
    [KSCOUT_ROLE_CENTRAL_DEFENDER_ST]         = "central-defender-st",
    [KSCOUT_ROLE_CENTRAL_DEFENDER_CO]         = "central-defender-co",
    [KSCOUT_ROLE_BALL_PLAYING_DEFENDER_D]     = "ball-playing-defender-d",
    [KSCOUT_ROLE_BALL_PLAYING_DEFENDER_ST]    = "ball-playing-defender-st",
    [KSCOUT_ROLE_BALL_PLAYING_DEFENDER_CO]    = "ball-playing-defender-co",
    [KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_D]   = "no-nonsense-centre-back-d",
    [KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_ST]  = "no-nonsense-centre-back-st",
    [KSCOUT_ROLE_NO_NONSENSE_CENTRE_BACK_CO]  = "no-nonsense-centre-back-co",
    [KSCOUT_ROLE_LIBERO_S]                    = "libero-s",
    [KSCOUT_ROLE_LIBERO_A]                    = "libero-a",

    [KSCOUT_ROLE_FULL_BACK_D]                 = "full-back-d",
    [KSCOUT_ROLE_FULL_BACK_S]                 = "full-back-s",
    [KSCOUT_ROLE_FULL_BACK_A]                 = "full-back-a",
    [KSCOUT_ROLE_DEFENSIVE_WING_BACK_D]       = "defensive-wing-back-d",

    [KSCOUT_ROLE_WING_BACK_D]                 = "wing-back-d",
    [KSCOUT_ROLE_WING_BACK_S]                 = "wing-back-s",
    [KSCOUT_ROLE_WING_BACK_A]                 = "wing-back-a",
    [KSCOUT_ROLE_COMPLETE_WING_BACK_S]        = "complete-wing-back-s",
    [KSCOUT_ROLE_COMPLETE_WING_BACK_A]        = "complete-wing-back-a",
    [KSCOUT_ROLE_INVERTED_WING_BACK_D]        = "inverted-wing-back-d",
    [KSCOUT_ROLE_INVERTED_WING_BACK_S]        = "inverted-wing-back-s",
    [KSCOUT_ROLE_INVERTED_WING_BACK_A]        = "inverted-wing-back-a",

    [KSCOUT_ROLE_ANCHOR_MAN_D]                = "anchor-man-d",
    [KSCOUT_ROLE_HALF_BACK_D]                 = "half-back-d",
    [KSCOUT_ROLE_BALL_WINNING_MIDFIELDER_D]   = "ball-winning-midfielder-d",
    [KSCOUT_ROLE_BALL_WINNING_MIDFIELDER_S]   = "ball-winning-midfielder-s",
    [KSCOUT_ROLE_DEFENSIVE_MIDFIELDER_D]      = "defensive-midfielder-d",
    [KSCOUT_ROLE_DEFENSIVE_MIDFIELDER_S]      = "defensive-midfielder-s",
    [KSCOUT_ROLE_DEEP_LYING_PLAYMAKER_D]      = "deep-lying-playmaker-d",
    [KSCOUT_ROLE_DEEP_LYING_PLAYMAKER_S]      = "deep-lying-playmaker-s",
    [KSCOUT_ROLE_SEGUNDO_VOLANTE_S]           = "segundo-volante-s",
    [KSCOUT_ROLE_SEGUNDO_VOLANTE_A]           = "segundo-volante-a",

    [KSCOUT_ROLE_CENTRAL_MIDFIELDER_D]        = "central-midfielder-d",
    [KSCOUT_ROLE_CENTRAL_MIDFIELDER_S]        = "central-midfielder-s",
    [KSCOUT_ROLE_CENTRAL_MIDFIELDER_A]        = "central-midfielder-a",
    [KSCOUT_ROLE_BOX_TO_BOX_MIDFIELDER_S]     = "box-to-box-midfielder-s",
    [KSCOUT_ROLE_CARRILERO_S]                 = "carrilero-s",
    [KSCOUT_ROLE_MEZZALA_S]                   = "mezzala-s",
    [KSCOUT_ROLE_MEZZALA_A]                   = "mezzala-a",
    [KSCOUT_ROLE_ROAMING_PLAYMAKER_S]         = "roaming-playmaker-s",

    [KSCOUT_ROLE_ADVANCED_PLAYMAKER_S]        = "advanced-playmaker-s",
    [KSCOUT_ROLE_ADVANCED_PLAYMAKER_A]        = "advanced-playmaker-a",
    [KSCOUT_ROLE_ATTACKING_MIDFIELDER_S]      = "attacking-midfielder-s",
    [KSCOUT_ROLE_ATTACKING_MIDFIELDER_A]      = "attacking-midfielder-a",
    [KSCOUT_ROLE_ENGANCHE_S]                  = "enganche-s",
    [KSCOUT_ROLE_SHADOW_STRIKER_A]            = "shadow-striker-a",
    [KSCOUT_ROLE_TREQUARTISTA_A]              = "trequartista-a",

    [KSCOUT_ROLE_WIDE_MIDFIELDER_D]           = "wide-midfielder-d",
    [KSCOUT_ROLE_WIDE_MIDFIELDER_S]           = "wide-midfielder-s",
    [KSCOUT_ROLE_WIDE_MIDFIELDER_A]           = "wide-midfielder-a",
    [KSCOUT_ROLE_WIDE_PLAYMAKER_S]            = "wide-playmaker-s",
    [KSCOUT_ROLE_WIDE_PLAYMAKER_A]            = "wide-playmaker-a",

    [KSCOUT_ROLE_WINGER_S]                    = "winger-s",
    [KSCOUT_ROLE_WINGER_A]                    = "winger-a",
    [KSCOUT_ROLE_INSIDE_FORWARD_S]            = "inside-forward-s",
    [KSCOUT_ROLE_INSIDE_FORWARD_A]            = "inside-forward-a",
    [KSCOUT_ROLE_INVERTED_WINGER_S]           = "inverted-winger-s",
    [KSCOUT_ROLE_INVERTED_WINGER_A]           = "inverted-winger-a",
    [KSCOUT_ROLE_RAUMDEUTER_A]                = "raumdeuter-a",
    [KSCOUT_ROLE_WIDE_TARGET_MAN_S]           = "wide-target-man-s",
    [KSCOUT_ROLE_WIDE_TARGET_MAN_A]           = "wide-target-man-a",

    [KSCOUT_ROLE_ADVANCED_FORWARD_A]          = "advanced-forward-a",
    [KSCOUT_ROLE_COMPLETE_FORWARD_S]          = "complete-forward-s",
    [KSCOUT_ROLE_COMPLETE_FORWARD_A]          = "complete-forward-a",
    [KSCOUT_ROLE_DEEP_LYING_FORWARD_S]        = "deep-lying-forward-s",
    [KSCOUT_ROLE_DEEP_LYING_FORWARD_A]        = "deep-lying-forward-a",
    [KSCOUT_ROLE_FALSE_NINE_S]                = "false-nine-s",
    [KSCOUT_ROLE_POACHER_A]                   = "poacher-a",
    [KSCOUT_ROLE_PRESSING_FORWARD_D]          = "pressing-forward-d",
    [KSCOUT_ROLE_PRESSING_FORWARD_S]          = "pressing-forward-s",
    [KSCOUT_ROLE_PRESSING_FORWARD_A]          = "pressing-forward-a",
    [KSCOUT_ROLE_TARGET_MAN_S]                = "target-man-s",
    [KSCOUT_ROLE_TARGET_MAN_A]                = "target-man-a",
    [KSCOUT_ROLE_TREQUARTISTA_ST_A]           = "trequartista-st-a",
};

struct kscout_role_score_s {
  const kscout_role_weights_t* def;
  float score;
};

struct kscout_role_rating_s {
  kscout_role_score_t* items;
  size_t count;
  size_t capacity;
};

struct kscout_role_weights_s {
  const char* name;
  char *valid_positions; // currently omitted
  int attribute_weights[KSCOUT_ATTR_COUNT];
  int max_weighted_score;
};

/* -------------------------------------------------------------------------
 * Lookup by config key. Returns KSCOUT_ROLE_COUNT on failure.
 * ---------------------------------------------------------------------- */

static inline kscout_role_t kscout_role_from_key(const char *key)
{
    if (!key) return KSCOUT_ROLE_COUNT;
    for (int i = 0; i < KSCOUT_ROLE_COUNT; i++) {
        if (kscout_role_keys[i] && strcmp(key, kscout_role_keys[i]) == 0)
            return (kscout_role_t)i;
    }
    return KSCOUT_ROLE_COUNT;
}

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_ROLE_H */