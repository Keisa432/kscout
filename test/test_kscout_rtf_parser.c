/* test_kscout_rtf_parser.c
 *
 * Unity tests for kscout_parser (pipe-delimited RTF shortlist export).
 *
 * Fixture files are written to /tmp at runtime so no external files are
 * needed – everything is self-contained in this translation unit.
 */

#include "kscout_parser.h"
#include "test_kscout.h"
#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* -----------------------------------------------------------------------
 * Fixture helpers
 * ----------------------------------------------------------------------- */

/* Write a string to a temp file and return the path.
 * Caller must remove() the file when done. */
static const char *write_fixture(const char *filename, const char *content)
{
  static char path[256];
  char cwd[128];
  if (!getcwd(cwd, sizeof(cwd)))
    return NULL;
  snprintf(path, sizeof(path), "%s/%s", cwd, filename);
  FILE *fp = fopen(path, "w");
  if (!fp)
    return NULL;
  fputs(content, fp);
  fclose(fp);
  return path;
}

/* -----------------------------------------------------------------------
 * Callback state – reset before each test
 * ----------------------------------------------------------------------- */
typedef struct
{
  int call_count;
  /* Snapshots of the first and last row received */
  char first_keys[64][64];
  char first_values[64][64];
  unsigned first_count;
  char last_keys[64][64];
  char last_values[64][64];
  unsigned last_count;
  unsigned max_token_count; /* widest row seen */
} cb_state_t;

static cb_state_t g_state;

static void reset_state(void) { memset(&g_state, 0, sizeof(g_state)); }

static void capture_cb(kscout_parser_token_t *tokens, unsigned count,
                       void *user)
{
  (void)user;
  /* Snapshot first row */
  if (g_state.call_count == 0)
  {
    g_state.first_count = count;
    for (unsigned i = 0; i < count && i < 64; i++)
    {
      strncpy(g_state.first_keys[i], tokens[i].key ? tokens[i].key : "", 63);
      strncpy(g_state.first_values[i], tokens[i].value ? tokens[i].value : "",
              63);
    }
  }
  /* Always overwrite "last" snapshot */
  g_state.last_count = count;
  for (unsigned i = 0; i < count && i < 64; i++)
  {
    strncpy(g_state.last_keys[i], tokens[i].key ? tokens[i].key : "", 63);
    strncpy(g_state.last_values[i], tokens[i].value ? tokens[i].value : "", 63);
  }
  if (count > g_state.max_token_count)
    g_state.max_token_count = count;
  g_state.call_count++;
}

/* -----------------------------------------------------------------------
 * Fixtures
 * ----------------------------------------------------------------------- */

/* Minimal two-row table with three columns */
#define FIXTURE_SIMPLE                                                         \
  "\n\n"                                                                       \
  "| Col1 | Col2 | Col3 |\n"                                                   \
  "| ---- | ---- | ---- |\n"                                                   \
  "| aaa  | bbb  | ccc  |\n"                                                   \
  "| ---- | ---- | ---- |\n"                                                   \
  "| ddd  | eee  | fff  |\n"                                                   \
  "| ---- | ---- | ---- |\n"

/* Table where the Inf column has an empty value (like the real export) */
#define FIXTURE_EMPTY_CELL                                                     \
  "| Inf | Name       | Age |\n"                                               \
  "| --- | ---------- | --- |\n"                                               \
  "|     | AliceSmith | 30  |\n"                                               \
  "| --- | ---------- | --- |\n"

/* Single data row – represents a real-world shortlist entry */
#define FIXTURE_REAL_ROW                                                       \
  "| Inf | Name              | Position | Nat | Age | Club       | Wage      " \
  " |\n"                                                                       \
  "| --- | ----------------- | -------- | --- | --- | ---------- | "           \
  "---------- |\n"                                                             \
  "|     | Julius Ertlthaler | M/AM (C) | AUT | 28  | WSG Tirol  | €4,300 "  \
  "p/w |\n"                                                                    \
  "| --- | ----------------- | -------- | --- | --- | ---------- | "           \
  "---------- |\n"

/* Table with leading blank lines and \r\n line endings (as the real RTF) */
#define FIXTURE_CRLF                                                           \
  "\r\n\r\n\r\n"                                                               \
  "| A | B |\r\n"                                                              \
  "| - | - |\r\n"                                                              \
  "| 1 | 2 |\r\n"                                                              \
  "| - | - |\r\n"                                                              \
  "| 3 | 4 |\r\n"                                                              \
  "| - | - |\r\n"

/* -----------------------------------------------------------------------
 * setUp / tearDown (called by Unity around each test)
 * ----------------------------------------------------------------------- */
void setUp(void) { reset_state(); }
void tearDown(void) {}

/* -----------------------------------------------------------------------
 * Tests – parser lifecycle
 * ----------------------------------------------------------------------- */

void test_parser_new_returns_zero_on_valid_args(void)
{
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  int rc = kscout_parser_new(&p, &cfg);
  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_NOT_NULL(p);
  kscout_parser_destroy(p);
}

void test_parser_new_fails_on_null_parser_ptr(void)
{
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  int rc = kscout_parser_new(NULL, &cfg);
  TEST_ASSERT_EQUAL_INT(-1, rc);
}

void test_parser_new_fails_on_null_cfg(void)
{
  kscout_parser_t *p = NULL;
  int rc = kscout_parser_new(&p, NULL);
  TEST_ASSERT_EQUAL_INT(-1, rc);
  TEST_ASSERT_NULL(p);
}

void test_parser_import_fails_on_missing_file(void)
{
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  int rc = kscout_parser_import(p, "/tmp/does_not_exist_kscout.rtf");
  TEST_ASSERT_EQUAL_INT(-1, rc);
  kscout_parser_destroy(p);
}

/* -----------------------------------------------------------------------
 * Tests – callback invocation count
 * ----------------------------------------------------------------------- */

void test_callback_called_once_per_data_row_simple(void)
{
  const char *path = write_fixture("simple.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_INT(2, g_state.call_count);
}

void test_callback_called_for_each_real_row(void)
{
  const char *path = write_fixture("real_single.rtf", FIXTURE_REAL_ROW);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_INT(1, g_state.call_count);
}

void test_callback_not_called_on_header_only_file(void)
{
  const char *content = "| Col1 | Col2 |\n"
                        "| ---- | ---- |\n";
  const char *path = write_fixture("header_only.rtf", content);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_INT(0, g_state.call_count);
}

/* -----------------------------------------------------------------------
 * Tests – token keys match header columns
 * ----------------------------------------------------------------------- */

void test_token_keys_match_header_columns(void)
{
  const char *path = write_fixture("keys.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_UINT(3, g_state.first_count);
  TEST_ASSERT_EQUAL_STRING("Col1", g_state.first_keys[0]);
  TEST_ASSERT_EQUAL_STRING("Col2", g_state.first_keys[1]);
  TEST_ASSERT_EQUAL_STRING("Col3", g_state.first_keys[2]);
}

void test_token_keys_are_same_for_every_row(void)
{
  /* Both first and last row should carry identical keys */
  const char *path = write_fixture("keys2.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_STRING(g_state.first_keys[0], g_state.last_keys[0]);
  TEST_ASSERT_EQUAL_STRING(g_state.first_keys[1], g_state.last_keys[1]);
  TEST_ASSERT_EQUAL_STRING(g_state.first_keys[2], g_state.last_keys[2]);
}

void test_real_header_columns_parsed_correctly(void)
{
  const char *path = write_fixture("real_keys.rtf", FIXTURE_REAL_ROW);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_STRING("Inf", g_state.first_keys[0]);
  TEST_ASSERT_EQUAL_STRING("Name", g_state.first_keys[1]);
  TEST_ASSERT_EQUAL_STRING("Position", g_state.first_keys[2]);
  TEST_ASSERT_EQUAL_STRING("Nat", g_state.first_keys[3]);
  TEST_ASSERT_EQUAL_STRING("Age", g_state.first_keys[4]);
  TEST_ASSERT_EQUAL_STRING("Club", g_state.first_keys[5]);
  TEST_ASSERT_EQUAL_STRING("Wage", g_state.first_keys[6]);
}

/* -----------------------------------------------------------------------
 * Tests – token values
 * ----------------------------------------------------------------------- */

void test_token_values_first_row(void)
{
  const char *path = write_fixture("vals1.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_STRING("aaa", g_state.first_values[0]);
  TEST_ASSERT_EQUAL_STRING("bbb", g_state.first_values[1]);
  TEST_ASSERT_EQUAL_STRING("ccc", g_state.first_values[2]);
}

void test_token_values_last_row(void)
{
  const char *path = write_fixture("vals2.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_STRING("ddd", g_state.last_values[0]);
  TEST_ASSERT_EQUAL_STRING("eee", g_state.last_values[1]);
  TEST_ASSERT_EQUAL_STRING("fff", g_state.last_values[2]);
}

void test_empty_cell_produces_empty_string_value(void)
{
  const char *path = write_fixture("empty_cell.rtf", FIXTURE_EMPTY_CELL);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  /* The Inf column for this player is blank */
  TEST_ASSERT_EQUAL_STRING("", g_state.first_values[0]);
  TEST_ASSERT_EQUAL_STRING("AliceSmith", g_state.first_values[1]);
  TEST_ASSERT_EQUAL_STRING("30", g_state.first_values[2]);
}

void test_real_row_values_parsed_correctly(void)
{
  const char *path = write_fixture("real_vals.rtf", FIXTURE_REAL_ROW);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_STRING("", g_state.first_values[0]); /* Inf empty */
  TEST_ASSERT_EQUAL_STRING("Julius Ertlthaler", g_state.first_values[1]);
  TEST_ASSERT_EQUAL_STRING("M/AM (C)", g_state.first_values[2]);
  TEST_ASSERT_EQUAL_STRING("AUT", g_state.first_values[3]);
  TEST_ASSERT_EQUAL_STRING("28", g_state.first_values[4]);
  TEST_ASSERT_EQUAL_STRING("WSG Tirol", g_state.first_values[5]);
  TEST_ASSERT_EQUAL_STRING("€4,300 p/w", g_state.first_values[6]);
}

/* -----------------------------------------------------------------------
 * Tests – token count
 * ----------------------------------------------------------------------- */

void test_token_count_matches_column_count(void)
{
  const char *path = write_fixture("cnt.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  /* 3 columns -> 3 tokens per row, for both rows */
  TEST_ASSERT_EQUAL_UINT(3, g_state.first_count);
  TEST_ASSERT_EQUAL_UINT(3, g_state.last_count);
}

/* -----------------------------------------------------------------------
 * Tests – robustness
 * ----------------------------------------------------------------------- */

void test_crlf_line_endings_handled(void)
{
  const char *path = write_fixture("crlf.rtf", FIXTURE_CRLF);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_INT(2, g_state.call_count);
  TEST_ASSERT_EQUAL_STRING("A", g_state.first_keys[0]);
  TEST_ASSERT_EQUAL_STRING("B", g_state.first_keys[1]);
  TEST_ASSERT_EQUAL_STRING("1", g_state.first_values[0]);
  TEST_ASSERT_EQUAL_STRING("2", g_state.first_values[1]);
  TEST_ASSERT_EQUAL_STRING("3", g_state.last_values[0]);
  TEST_ASSERT_EQUAL_STRING("4", g_state.last_values[1]);
}

void test_leading_blank_lines_skipped(void)
{
  /* Five blank lines before the header – must still parse correctly */
  const char *content = "\n\n\n\n\n"
                        "| X | Y |\n"
                        "| - | - |\n"
                        "| 9 | 8 |\n"
                        "| - | - |\n";
  const char *path = write_fixture("blanks.rtf", content);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  TEST_ASSERT_EQUAL_INT(1, g_state.call_count);
  TEST_ASSERT_EQUAL_STRING("X", g_state.first_keys[0]);
  TEST_ASSERT_EQUAL_STRING("9", g_state.first_values[0]);
}

void test_separator_lines_not_passed_to_callback(void)
{
  /* All separator lines must be silently skipped */
  const char *path = write_fixture("sep.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);

  /* FIXTURE_SIMPLE has 2 data rows and 3 separator lines; only 2 callbacks */
  TEST_ASSERT_EQUAL_INT(2, g_state.call_count);
}

void test_import_returns_zero_on_success(void)
{
  const char *path = write_fixture("rc.rtf", FIXTURE_SIMPLE);
  kscout_parser_cfg_t cfg = {.type = KSCOUT_PARSER_RTF, .cb = capture_cb};
  kscout_parser_t *p = NULL;
  kscout_parser_new(&p, &cfg);
  int rc = kscout_parser_import(p, (char *)path);
  kscout_parser_destroy(p);
  remove(path);
  TEST_ASSERT_EQUAL_INT(0, rc);
}
