#ifndef KSCOUT_DA_H
#define KSCOUT_DA_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KSCOUT_DA_INIT_CAP 32

#define __kscout_da_resize(da, new_count)                                      \
  do {                                                                         \
    if ((new_count) > (da)->capacity) {                                        \
      if ((da)->capacity == 0)                                                 \
        (da)->capacity = KSCOUT_DA_INIT_CAP;                                   \
      while ((new_count) > (da)->capacity) {                                   \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items = realloc((da)->items, sizeof(*(da)->items) * da->capacity); \
    }                                                                          \
  } while (0)

#define kscout_da_push(da, item)                                               \
  do {                                                                         \
    __kscout_da_resize((da), ((da)->count + 1));                               \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define kscout_da_pop(da, item)                                                \
  do {                                                                         \
    if ((da)->count > 0) {                                                     \
      (item) = (da)->items[--(da)->count];                                     \
    }                                                                          \
  } while (0)

#define kscout_da_free(da) free(da.items)

#define kscout_da_foreach(Type, it, da) for (Type *it = (da)->items; it < (da)->items + (da)->count; it++)

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_DA_H