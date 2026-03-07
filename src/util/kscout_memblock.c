#include "kscout_memblock.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Round n up to the nearest multiple of KSCOUT_MEMBLOCK_ALIGN. */
static inline size_t align_up(size_t n)
{
  const size_t mask = (size_t)(KSCOUT_MEMBLOCK_ALIGN - 1);
  return (n + mask) & ~mask;
}

/*
 * Allocate a new slab whose payload is at least `capacity` bytes.
 * The slab is not linked into any list here; the caller does that.
 *
 * Returns NULL on failure (errno set by malloc).
 */
static kscout_memblock_slab_t *slab_new(size_t capacity)
{
  /* sizeof the header + flexible array member payload */
  size_t total = sizeof(kscout_memblock_slab_t) + capacity;
  if (total < capacity) { /* overflow */
    errno = ENOMEM;
    return NULL;
  }

  kscout_memblock_slab_t *slab = (kscout_memblock_slab_t *)malloc(total);
  if (!slab)
    return NULL;

  slab->next = NULL;
  slab->capacity = capacity;
  slab->used = 0;
  return slab;
}

/*
 * Try to carve `aligned_size` bytes from `slab`.
 * Returns a pointer into slab->data, or NULL if there is not enough room.
 */
static void *slab_try_alloc(kscout_memblock_slab_t *slab, size_t aligned_size)
{
  if (slab->used + aligned_size > slab->capacity)
    return NULL;

  void *ptr = slab->data + slab->used;
  slab->used += aligned_size;
  return ptr;
}

/* ------------------------------------------------------------------ */
/*  Lifecycle                                                           */
/* ------------------------------------------------------------------ */

int kscout_memblock_init(kscout_memblock_t *mb, size_t slab_size)
{
  if (!mb || slab_size == 0) {
    errno = EINVAL;
    return -1;
  }

  mb->head = NULL;
  mb->current = NULL;
  mb->slab_size = align_up(slab_size); /* keep slab size aligned too */
  return 0;
}

void kscout_memblock_destroy(kscout_memblock_t *mb)
{
  if (!mb)
    return;

  kscout_memblock_slab_t *slab = mb->head;
  while (slab) {
    kscout_memblock_slab_t *next = slab->next;
    free(slab);
    slab = next;
  }

  mb->head = NULL;
  mb->current = NULL;
  /* leave slab_size intact so the struct can be re-initialised */
}

void kscout_memblock_reset(kscout_memblock_t *mb)
{
  if (!mb || !mb->head)
    return;

  /* Free all slabs after the first. */
  kscout_memblock_slab_t *slab = mb->head->next;
  while (slab) {
    kscout_memblock_slab_t *next = slab->next;
    free(slab);
    slab = next;
  }

  mb->head->next = NULL;
  mb->head->used = 0;
  mb->current = mb->head;
}

/* ------------------------------------------------------------------ */
/*  Core allocator                                                      */
/* ------------------------------------------------------------------ */

void *kscout_memblock_alloc(kscout_memblock_t *mb, size_t size)
{
  if (!mb || size == 0)
    return NULL;

  size_t aligned = align_up(size);
  if (aligned < size) { /* overflow in align_up */
    errno = ENOMEM;
    return NULL;
  }

  /* Fast path: try the current slab first. */
  if (mb->current) {
    void *ptr = slab_try_alloc(mb->current, aligned);
    if (ptr)
      return ptr;
  }

  /*
   * Current slab is full (or doesn't exist yet).
   * Allocate a new slab large enough for this request.
   * Oversized requests get their own dedicated slab.
   */
  size_t new_cap = (aligned > mb->slab_size) ? aligned : mb->slab_size;
  kscout_memblock_slab_t *slab = slab_new(new_cap);
  if (!slab)
    return NULL;

  /* Link the new slab at the end of the chain. */
  if (!mb->head) {
    mb->head = slab;
  } else {
    mb->current->next = slab;
  }
  mb->current = slab;

  /* This must succeed: we just allocated exactly enough room. */
  return slab_try_alloc(slab, aligned);
}

/* ------------------------------------------------------------------ */
/*  Convenience API                                                     */
/* ------------------------------------------------------------------ */

void *kscout_memblock_malloc(kscout_memblock_t *mb, size_t size)
{
  return kscout_memblock_alloc(mb, size);
}

void *kscout_memblock_calloc(kscout_memblock_t *mb, size_t nmemb, size_t size)
{
  if (!mb)
    return NULL;

  /* Detect multiplication overflow. */
  if (nmemb != 0 && size > SIZE_MAX / nmemb) {
    errno = ENOMEM;
    return NULL;
  }

  size_t total = nmemb * size;
  void *ptr = kscout_memblock_alloc(mb, total);
  if (ptr)
    memset(ptr, 0, total);
  return ptr;
}

char *kscout_memblock_strdup(kscout_memblock_t *mb, const char *s)
{
  if (!mb || !s)
    return NULL;

  size_t len = strlen(s) + 1;
  char *dst = (char *)kscout_memblock_alloc(mb, len);
  if (dst)
    memcpy(dst, s, len);
  return dst;
}

/* ------------------------------------------------------------------ */
/*  Diagnostics                                                         */
/* ------------------------------------------------------------------ */

size_t kscout_memblock_used(const kscout_memblock_t *mb)
{
  if (!mb)
    return 0;

  size_t total = 0;
  for (kscout_memblock_slab_t *s = mb->head; s; s = s->next)
    total += s->used;
  return total;
}

size_t kscout_memblock_capacity(const kscout_memblock_t *mb)
{
  if (!mb)
    return 0;

  size_t total = 0;
  for (kscout_memblock_slab_t *s = mb->head; s; s = s->next)
    total += s->capacity;
  return total;
}

size_t kscout_memblock_slab_count(const kscout_memblock_t *mb)
{
  if (!mb)
    return 0;

  size_t count = 0;
  for (kscout_memblock_slab_t *s = mb->head; s; s = s->next)
    count++;
  return count;
}