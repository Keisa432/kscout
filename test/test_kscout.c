#include <unity.h>
#include "test_kscout.h"


int main(void) {
    UNITY_BEGIN();
    RUN_ALL_KSCOUT_PARSER_TESTS();
    return UNITY_END();
}