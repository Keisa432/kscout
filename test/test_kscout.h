#ifndef TEST_KSCOUT_H
#define TEST_KSCOUT_H

/*****************************************************************************/
/* RTF parser tests                                                          */

/* --- lifecycle --- */
void test_parser_new_returns_zero_on_valid_args(void);
void test_parser_new_fails_on_null_parser_ptr(void);
void test_parser_new_fails_on_null_cfg(void);
void test_parser_import_fails_on_missing_file(void);

/* --- callback invocation count --- */
void test_callback_called_once_per_data_row_simple(void);
void test_callback_called_for_each_real_row(void);
void test_callback_not_called_on_header_only_file(void);

/* --- token keys --- */
void test_token_keys_match_header_columns(void);
void test_token_keys_are_same_for_every_row(void);
void test_real_header_columns_parsed_correctly(void);

/* --- token values --- */
void test_token_values_first_row(void);
void test_token_values_last_row(void);
void test_empty_cell_produces_empty_string_value(void);
void test_real_row_values_parsed_correctly(void);

/* --- token count --- */
void test_token_count_matches_column_count(void);

/* --- robustness --- */
void test_crlf_line_endings_handled(void);
void test_leading_blank_lines_skipped(void);
void test_separator_lines_not_passed_to_callback(void);
void test_import_returns_zero_on_success(void);

/* Convenience macro – paste this block into any runner's main() to run all tests:
 *
 *   #include "test_kscout.h"
 *   ...
 *   UNITY_BEGIN();
 *   RUN_ALL_KSCOUT_TESTS();
 *   return UNITY_END();
 */
#define RUN_ALL_KSCOUT_PARSER_TESTS() \
  RUN_TEST(test_parser_new_returns_zero_on_valid_args);       \
  RUN_TEST(test_parser_new_fails_on_null_parser_ptr);         \
  RUN_TEST(test_parser_new_fails_on_null_cfg);                \
  RUN_TEST(test_parser_import_fails_on_missing_file);         \
  RUN_TEST(test_callback_called_once_per_data_row_simple);    \
  RUN_TEST(test_callback_called_for_each_real_row);           \
  RUN_TEST(test_callback_not_called_on_header_only_file);     \
  RUN_TEST(test_token_keys_match_header_columns);             \
  RUN_TEST(test_token_keys_are_same_for_every_row);           \
  RUN_TEST(test_real_header_columns_parsed_correctly);        \
  RUN_TEST(test_token_values_first_row);                      \
  RUN_TEST(test_token_values_last_row);                       \
  RUN_TEST(test_empty_cell_produces_empty_string_value);      \
  RUN_TEST(test_real_row_values_parsed_correctly);            \
  RUN_TEST(test_token_count_matches_column_count);            \
  RUN_TEST(test_crlf_line_endings_handled);                   \
  RUN_TEST(test_leading_blank_lines_skipped);                 \
  RUN_TEST(test_separator_lines_not_passed_to_callback);      \
  RUN_TEST(test_import_returns_zero_on_success)

#endif // TEST_KSCOUT_H