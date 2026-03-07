#ifndef KSCOUT_MEMBLOCK_H
#define KSCOUT_MEMBLOCK_H

/*
 * kscout_memblock — A growable arena allocator with stable pointers.
 *
 * Memory is organised as a singly-linked list of fixed-size slabs.
 * When the current slab is exhausted a new one is malloc'd and chained on;
 * existing slabs are NEVER realloc'd, so every pointer returned by this
 * allocator remains valid for the lifetime of the kscout_memblock_t.
 *
 * Trade-offs vs. a single-buffer realloc design:
 *   + Pointers are permanently stable — no dangling pointer hazard.
 *   + No data is copied on growth.
 *   - Slight per-slab overhead (one malloc + header).
 *   - Memory is not contiguous across slab boundaries.
 *
 * Individual frees are not supported; reclaim memory with
 * kscout_memblock_destroy() or kscout_memblock_reset().
 *
 * Alignment: every allocation is padded to KSCOUT_MEMBLOCK_ALIGN bytes
 * (default 8 — safe for all scalar C types, including double / int64_t).
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Configuration                                                       */
/* ------------------------------------------------------------------ */

#ifndef KSCOUT_MEMBLOCK_ALIGN
#  define KSCOUT_MEMBLOCK_ALIGN 8
#endif

/* ------------------------------------------------------------------ */
/*  Internal slab (treat as opaque)                                     */
/* ------------------------------------------------------------------ */

typedef struct kscout_memblock_slab {
    struct kscout_memblock_slab *next;     /* linked-list chain            */
    size_t                       capacity; /* usable bytes in data[]       */
    size_t                       used;     /* bytes consumed so far        */
    char                         data[];   /* flexible array — payload     */
} kscout_memblock_slab_t;

/* ------------------------------------------------------------------ */
/*  Public handle                                                       */
/* ------------------------------------------------------------------ */

typedef struct kscout_memblock {
    kscout_memblock_slab_t *head;      /* first slab in the chain          */
    kscout_memblock_slab_t *current;   /* slab we are currently filling    */
    size_t                  slab_size; /* default capacity for new slabs   */
} kscout_memblock_t;

/* ------------------------------------------------------------------ */
/*  Lifecycle                                                           */
/* ------------------------------------------------------------------ */

/**
 * kscout_memblock_init - Initialise a memblock.
 *
 * @mb:        Pointer to a caller-owned kscout_memblock_t.
 * @slab_size: Default capacity (in bytes) of each slab allocated on demand.
 *             A request larger than slab_size is served by a dedicated
 *             oversized slab so it always succeeds.
 *             Must be > 0.
 *
 * No heap allocation is done here; the first slab is created lazily on the
 * first call to kscout_memblock_alloc().
 *
 * Returns 0 on success, -1 if arguments are invalid (errno = EINVAL).
 */
int kscout_memblock_init(kscout_memblock_t *mb, size_t slab_size);

/**
 * kscout_memblock_destroy - Free every slab and zero the handle.
 *
 * All pointers previously returned by this allocator become invalid.
 */
void kscout_memblock_destroy(kscout_memblock_t *mb);

/**
 * kscout_memblock_reset - Rewind the arena without releasing OS memory.
 *
 * Frees all slabs except the first, then resets its used counter to 0.
 * All previously returned pointers become invalid after this call.
 * Useful for reprocessing loops that allocate-and-discard repeatedly.
 */
void kscout_memblock_reset(kscout_memblock_t *mb);

/* ------------------------------------------------------------------ */
/*  Core allocator                                                      */
/* ------------------------------------------------------------------ */

/**
 * kscout_memblock_alloc - Allocate 'size' bytes from the arena.
 *
 * The returned pointer is aligned to KSCOUT_MEMBLOCK_ALIGN bytes and
 * remains valid until kscout_memblock_destroy() or kscout_memblock_reset().
 *
 * Returns NULL on allocation failure or if size == 0.
 */
void *kscout_memblock_alloc(kscout_memblock_t *mb, size_t size);

/* ------------------------------------------------------------------ */
/*  Convenience API                                                     */
/* ------------------------------------------------------------------ */

/** Like malloc(size) — content is undefined (not zeroed). */
void *kscout_memblock_malloc(kscout_memblock_t *mb, size_t size);

/** Like calloc(nmemb, size) — memory is zeroed; overflow-safe. */
void *kscout_memblock_calloc(kscout_memblock_t *mb, size_t nmemb, size_t size);

/** Like strdup(s) — copies the NUL-terminated string into the arena. */
char *kscout_memblock_strdup(kscout_memblock_t *mb, const char *s);

/* ------------------------------------------------------------------ */
/*  Diagnostics                                                         */
/* ------------------------------------------------------------------ */

/** Total bytes in use across all slab payloads. */
size_t kscout_memblock_used(const kscout_memblock_t *mb);

/** Total payload capacity across all slabs. */
size_t kscout_memblock_capacity(const kscout_memblock_t *mb);

/** Number of slabs currently allocated. */
size_t kscout_memblock_slab_count(const kscout_memblock_t *mb);

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_MEMBLOCK_H */