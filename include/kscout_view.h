#ifndef KSCOUT_view_H
#define KSCOUT_view_H

#include "kscout_error.h"
#include "kscout_player.h"
#include "kscout_memblock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kscout_view_s kscout_view_t;
typedef struct kscout_view_iter_s kscout_view_iter_t;

int kscout_view_new(kscout_view_t **db, kscout_scouter_t *scouter);
void kscout_view_destroy(kscout_view_t *db);

int kscout_view_load_file(kscout_view_t **db, const char *path);
int kscout_view_export_to_json(kscout_view_t* view, const char* path);

int kscout_view_iter_init(kscout_view_t *db, kscout_view_iter_t **it);
int kscout_view_iter_next(kscout_view_iter_t *it, kscout_player_t *player);
void kscout_view_iter_destroy(kscout_view_iter_t *it);

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_view_H