#ifndef KSCOUT_DA_H
#define KSCOUT_DA_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_DA_INIT_CAP 32

/**
 * @brief Defines a dynamic array struct for elements of type @p mtype.
 *
 * @param type  Name of the struct to define.
 * @param mtype Element type stored in the array.
 * @param name  Public alias for the internal @c _items pointer.
 *
 * The generated struct exposes the element pointer under both @p name and the
 * private @c _items alias (used internally by the dynamic-array macros).
 * Direct access through @c _items is discouraged; prefer the named alias.
 *
 * Members:
 *   - @p name / @c _items — pointer to the allocated element buffer
 *   - @c count            — number of elements currently in the array
 *   - @c capacity         — number of elements allocated
 *
 * @par Example
 * @code
 *   KSCOUT_DA_DEFINE(PlayerArray, Player, players);
 *   // struct PlayerArray {
 *   //   union { Player *players; Player *_items; };
 *   //   size_t count;
 *   //   size_t capacity;
 *   // }
 * @endcode
 */
#define KSCOUT_DA_DEFINE(type, mtype, name)                                    \
  struct type {                                                                \
    union {                                                                    \
      mtype *name;                                                             \
      mtype *_items;                                                           \
    };                                                                         \
    size_t count;                                                              \
    size_t capacity;                                                           \
  }

#define __kscout_da_resize(da, new_count)                                      \
  do {                                                                         \
    if ((new_count) > (da)->capacity) {                                        \
      if ((da)->capacity == 0)                                                 \
        (da)->capacity = KSCOUT_DA_INIT_CAP;                                   \
      while ((new_count) > (da)->capacity) {                                   \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->_items =                                                           \
          realloc((da)->_items, sizeof((da)->_items[0]) * da->capacity);       \
    }                                                                          \
  } while (0)

#define kscout_da_push(da, item)                                               \
  do {                                                                         \
    __kscout_da_resize((da), ((da)->count + 1));                               \
    (da)->_items[(da)->count++] = (item);                                      \
  } while (0)

#define kscout_da_pop(da, item)                                                \
  do {                                                                         \
    if ((da)->count > 0) {                                                     \
      (item) = (da)->_items[--(da)->count];                                    \
    }                                                                          \
  } while (0)

#define kscout_da_free(da) free(da._items)

#define kscout_da_foreach(Type, it, da)                                        \
  for (Type *it = (da)->_items; it < (da)->_items + (da)->count; it++)

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_DA_H
