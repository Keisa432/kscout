#ifndef KSCOUT_SCORING_H
#define KSCOUT_SCORING_H

#include "kscout_error.h"
#include "kscout_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kscout_attr_rating_s kscout_attr_rating_t;
typedef struct kscout_scouter_s kscout_scouter_t;
typedef struct kscout_scouter_report_s kscout_scouter_report_t;

struct kscout_attr_rating_s {
  float technical;
  float mental;
  float physical;
};

struct kscout_scouter_report_s {
  kscout_player_t player;
  kscout_attr_rating_t attr;
  kscout_role_rating_t role_rating;
};


static const char *kscout_scoring_cfg_attr_keys[] = {
    [KSCOUT_ATTR_1V1]  = "1v1",
    [KSCOUT_ATTR_ACC]  = "Acc",
    [KSCOUT_ATTR_AER]  = "Aer",
    [KSCOUT_ATTR_AGG]  = "Agg",
    [KSCOUT_ATTR_AGI]  = "Agi",
    [KSCOUT_ATTR_ANT]  = "Ant",
    [KSCOUT_ATTR_BAL]  = "Bal",
    [KSCOUT_ATTR_BRA]  = "Bra",
    [KSCOUT_ATTR_CMD]  = "Cmd",
    [KSCOUT_ATTR_CNT]  = "Cnt",
    [KSCOUT_ATTR_COM]  = "Com",
    [KSCOUT_ATTR_CMP]  = "Cmp",
    [KSCOUT_ATTR_COR]  = "Cor",
    [KSCOUT_ATTR_CRO]  = "Cro",
    [KSCOUT_ATTR_DEC]  = "Dec",
    [KSCOUT_ATTR_DET]  = "Det",
    [KSCOUT_ATTR_ECC]  = "Ecc",
    [KSCOUT_ATTR_DRI]  = "Dri",
    [KSCOUT_ATTR_FIN]  = "Fin",
    [KSCOUT_ATTR_FIR]  = "Fir",
    [KSCOUT_ATTR_FRE]  = "Fre",
    [KSCOUT_ATTR_FLA]  = "Fla",
    [KSCOUT_ATTR_HAN]  = "Han",
    [KSCOUT_ATTR_HEA]  = "Hea",
    [KSCOUT_ATTR_JUM]  = "Jum",
    [KSCOUT_ATTR_KIC]  = "Kic",
    [KSCOUT_ATTR_LDR]  = "Ldr",
    [KSCOUT_ATTR_LON]  = "Lon",
    [KSCOUT_ATTR_L_TH] = "L Th",
    [KSCOUT_ATTR_MAR]  = "Mar",
    [KSCOUT_ATTR_NAT]  = "Nat",
    [KSCOUT_ATTR_OTB]  = "OtB",
    [KSCOUT_ATTR_PAC]  = "Pac",
    [KSCOUT_ATTR_PAS]  = "Pas",
    [KSCOUT_ATTR_POS]  = "Pos",
    [KSCOUT_ATTR_REF]  = "Ref",
    [KSCOUT_ATTR_STA]  = "Sta",
    [KSCOUT_ATTR_STR]  = "Str",
    [KSCOUT_ATTR_TCK]  = "Tck",
    [KSCOUT_ATTR_TEA]  = "Tea",
    [KSCOUT_ATTR_TEC]  = "Tec",
    [KSCOUT_ATTR_THR]  = "Thr",
    [KSCOUT_ATTR_PEN]  = "Pen",
    [KSCOUT_ATTR_PUN]  = "Pun",
    [KSCOUT_ATTR_TRO]  = "TRO",
    [KSCOUT_ATTR_VIS]  = "Vis",
    [KSCOUT_ATTR_WOR]  = "Wor",
};


int kscout_scouter_new(kscout_scouter_t **scouter, const char *cfg_path);

void kscout_scouter_destory(kscout_scouter_t *scouter);

int kscout_scouter_player_rate(kscout_scouter_t *scouter,
                               kscout_player_t *player);

static inline kscout_attr_t kscout_scouter_attr_by_key(const char *key)
{
  if (!key) {
    return KSCOUT_ATTR_COUNT;
  }

  for (int i = 0; i < KSCOUT_ATTR_COUNT; i++) {
    if (kscout_scoring_cfg_attr_keys[i] &&
        strcmp(key, kscout_scoring_cfg_attr_keys[i]) == 0) {
      return (kscout_attr_t) i;
    }
  }

  return KSCOUT_ATTR_COUNT;
}

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_SCORING_H