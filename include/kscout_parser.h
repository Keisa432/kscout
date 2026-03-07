#ifndef KSCOUT_PARSER_H
#define KSCOUT_PARSER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kscout_parser_s kscout_parser_t;
typedef struct kscout_parser_cfg_s kscout_parser_cfg_t;
typedef struct kscout_parser_token_s kscout_parser_token_t;

typedef void (*kscout_parser_proc_cb_t)(kscout_parser_token_t *tokens,
                                        unsigned count, void *user);

typedef enum { KSCOUT_PARSER_RTF = 0 } kscout_parser_type_t;

struct kscout_parser_cfg_s {
  kscout_parser_type_t type;
  kscout_parser_proc_cb_t cb;
  void *cb_arg;
};

struct kscout_parser_token_s {
  char *key;   /* Points into the stored header line – do not free */
  char *value; /* Points into a per-row scratch buffer – valid only during the
                  callback */
};

/* Lifecycle */
int kscout_parser_new(kscout_parser_t **parser, kscout_parser_cfg_t *cfg);
void kscout_parser_destroy(kscout_parser_t *parser);

/* Import a pipe-delimited RTF shortlist export */
int kscout_parser_import(kscout_parser_t *parser, const char *path);

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_PARSER_H */
