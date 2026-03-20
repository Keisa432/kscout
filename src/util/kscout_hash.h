#ifndef KSCOUT_HASH_H
#define KSCOUT_HASH_H

/*
 * kscout_hash — Fixed-capacity string hash map with open addressing.
 *
 * The table is allocated once via kscout_hash_init() and never resized.
 * Keys are const char pointers and must remain valid for the table's lifetime.
 * Values are stored as void pointers.
 *
 * Collision resolution uses linear probing. kscout_hash_put() returns
 * KSCOUT_ERR_FULL when the table is at capacity instead of resizing.
 */

#include "kscout_da.h"
#include "kscout_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kscout_hash_s kscout_hash_t;
typedef struct kscout_hash_slot_s kscout_hash_slot_t;

struct kscout_hash_slot_s {
  const char *key;
  void *item;
};

KSCOUT_DA_DEFINE(kscout_hash_s, kscout_hash_slot_t, slots);

/** Allocate and zero-initialise a table with room for at least @p count entries. */
int kscout_hash_init(kscout_hash_t *hash, size_t count);

/** Return the value for @p key, or NULL if not found. */
void *kscout_hash_get(kscout_hash_t *hash, const char *key);

/**
 * Insert @p key / @p item.
 * Returns KSCOUT_ERR_ALREADY_EXISTS if the key is present,
 * KSCOUT_ERR_FULL if the table has no free slots.
 */
int kscout_hash_put(kscout_hash_t *hash, const char *key, void *item);

/** Free the slot buffer and reset the table to an empty state. */
void kscout_hash_destroy(kscout_hash_t *hash);


#ifdef __cplusplus
}
#endif

#endif // KSCOUT_HASH_H
