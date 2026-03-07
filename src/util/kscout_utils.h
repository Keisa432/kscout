#ifndef KSCOUT_UTILS_H
#define KSCOUT_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int has_suffix(const char *s, const char *suffix);

const char *trim(const char *s, char *buf, size_t buflen);
#define TRIM(s) trim((s), _tbuf, sizeof(_tbuf))

#ifdef __cplusplus
}
#endif

#endif // KSCOUT_UTILS_H