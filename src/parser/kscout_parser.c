#include "kscout_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KSCOUT_MAX_TOKENS 128
#define KSCOUT_LINE_BUF 4096

struct kscout_parser_s {
  kscout_parser_token_t token[KSCOUT_MAX_TOKENS]; /* reused per row, keys point
                                                     into header_line */
  int header_parsed;
  char header_line[KSCOUT_LINE_BUF];
  char *header_keys[KSCOUT_MAX_TOKENS];
  int header_token_cnt;
  kscout_parser_proc_cb_t cb;
  void *cb_arg;
};

/* --------------------------------------------------------------------------
 * Internal helpers
 * -------------------------------------------------------------------------- */

/* Trim leading/trailing whitespace in-place and return the pointer. */
static char *trim(char *s)
{
  while (*s && isspace((unsigned char)*s))
    s++;
  char *end = s + strlen(s);
  while (end > s && isspace((unsigned char)*(end - 1)))
    end--;
  *end = '\0';
  return s;
}

/* Returns non-zero if the line is a separator line (all dashes / pipes). */
static int is_separator_line(const char *line)
{
  for (const char *p = line; *p; p++) {
    if (*p != '-' && *p != '|' && !isspace((unsigned char)*p) && *p != '\r' &&
        *p != '\n') {
      return 0;
    }
  }
  /* Must contain at least one dash to be a real separator. */
  return strchr(line, '-') != NULL;
}

/* Returns non-zero if the line is empty or contains only whitespace/CR/LF. */
static int is_blank_line(const char *line)
{
  for (const char *p = line; *p; p++) {
    if (!isspace((unsigned char)*p))
      return 0;
  }
  return 1;
}

/*
 * Tokenise a pipe-delimited row into `out` (array of char* pointing into
 * a writable copy of the line).  Returns the number of tokens found.
 *
 * The line is expected to look like:
 *   | val1 | val2 | val3 |
 *
 * Each token value is trimmed in-place.  `buf` must be a writable buffer
 * containing the line (it will be modified by strtok-style splitting).
 */
static int tokenize_pipe_line(char *buf, char **out, int max_out)
{
  int cnt = 0;
  char *p = buf;

  /* Skip leading whitespace */
  while (*p && isspace((unsigned char)*p))
    p++;

  /* Must start with a pipe */
  if (*p != '|')
    return 0;
  p++; /* skip first '|' */

  while (*p && cnt < max_out) {
    /* Find the closing pipe for this cell */
    char *cell_start = p;
    char *pipe = strchr(p, '|');
    if (!pipe)
      break;

    *pipe = '\0'; /* terminate cell */
    out[cnt++] = trim(cell_start);
    p = pipe + 1; /* advance past the pipe we just consumed */
  }

  return cnt;
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int kscout_parser_new(kscout_parser_t **parser, kscout_parser_cfg_t *cfg)
{
  if (!parser || !cfg) {
    return -1;
  }

  kscout_parser_t *p = calloc(1, sizeof(*p));
  if (!p) {
    return -1;
  }

  p->cb = cfg->cb;
  p->cb_arg = cfg->cb_arg;

  *parser = p;
  return 0;
}

void kscout_parser_destroy(kscout_parser_t *parser) { free(parser); }

int kscout_parser_import(kscout_parser_t *parser, const char *path)
{
  if (!parser || !path) {
    return -1;
  }

  FILE *fp = fopen(path, "r");
  if (!fp) {
    return -1;
  }

  char line[KSCOUT_LINE_BUF];
  /* Scratch buffer for tokenising data rows without clobbering header_line. */
  char row_buf[KSCOUT_LINE_BUF];

  while (fgets(line, sizeof(line), fp)) {
    /* Skip blank lines and separator lines unconditionally. */
    if (is_blank_line(line)) {
      continue;
    }
    if (is_separator_line(line)) {
      continue;
    }

    if (!parser->header_parsed) {
      /*
       * First meaningful non-separator line is the header.
       * Store a copy in header_line and tokenise it.
       */
      strncpy(parser->header_line, line, sizeof(parser->header_line) - 1);
      parser->header_line[sizeof(parser->header_line) - 1] = '\0';

      parser->header_token_cnt = tokenize_pipe_line(
          parser->header_line, parser->header_keys, KSCOUT_MAX_TOKENS);

      parser->header_parsed = 1;
    } else {
      /*
       * Data row: tokenise into a scratch buffer, pair each value with the
       * corresponding header key, then fire the callback.
       */
      strncpy(row_buf, line, sizeof(row_buf) - 1);
      row_buf[sizeof(row_buf) - 1] = '\0';

      char *values[KSCOUT_MAX_TOKENS];
      int val_cnt = tokenize_pipe_line(row_buf, values, KSCOUT_MAX_TOKENS);

      /* Use the lesser of header columns and actual value columns. */
      int token_cnt = val_cnt < parser->header_token_cnt
                          ? val_cnt
                          : parser->header_token_cnt;

      for (int i = 0; i < token_cnt; i++) {
        parser->token[i].key = parser->header_keys[i];
        parser->token[i].value = values[i];
      }

      if (parser->cb && token_cnt > 0) {
        parser->cb(parser->token, (unsigned)token_cnt, parser->cb_arg);
      }
    }
  }

  fclose(fp);
  return 0;
}
