#include "kscout_hash.h"
#include <stdint.h>
#include <string.h>

static uint32_t kscout_hash_djb2(const char *key)
{
  uint32_t hash = 5381;
  int c;
  while ((c = (unsigned char)*key++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void *kscout_hash_get(kscout_hash_t *hash, const char *key)
{
  if (!hash || !key || hash->capacity == 0) {
    return NULL;
  }

  size_t index = kscout_hash_djb2(key) % hash->capacity;
  for (size_t i = 0; i < hash->capacity; i++) {
    kscout_hash_slot_t *slot = &hash->slots[(index + i) % hash->capacity];
    if (!slot->key) {
      return NULL;
    }
    if (strcmp(slot->key, key) == 0) {
      return slot->item;
    }
  }
  return NULL;
}

int kscout_hash_put(kscout_hash_t *hash, const char *key, void *item)
{
  if (!hash || !key) {
    return KSCOUT_ERR_INVALID;
  }

  if (hash->count >= hash->capacity) {
    return KSCOUT_ERR_FULL;
  }

  size_t index = kscout_hash_djb2(key) % hash->capacity;
  for (size_t i = 0; i < hash->capacity; i++) {
    kscout_hash_slot_t *slot = &hash->slots[(index + i) % hash->capacity];
    if (!slot->key) {
      slot->key = key;
      slot->item = item;
      hash->count++;
      return KSCOUT_OK;
    }
    if (strcmp(slot->key, key) == 0) {
      return KSCOUT_ERR_ALREADY_EXISTS;
    }
  }
  return KSCOUT_ERR_FULL;
}

int kscout_hash_init(kscout_hash_t *hash, size_t count)
{
  if (!hash || count == 0) {
    return KSCOUT_ERR_INVALID;
  }

  __kscout_da_resize(hash, count);
  memset(hash->slots, 0, sizeof(hash->slots[0]) * hash->capacity);
  hash->count = 0;

  return KSCOUT_OK;
}

void kscout_hash_destroy(kscout_hash_t *hash)
{
  if (!hash) {
    return;
  }
  free(hash->slots);
  hash->slots = NULL;
  hash->count = 0;
  hash->capacity = 0;
}
