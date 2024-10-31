#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "nobu.h"

#define TEST(TEST_NAME) if (run_test(TEST_NAME, argc))

int run_test(const char* test_name, int argc) {
    /* we run every test by default */
    if (argc == 1) return 1;
    return 0;
}

int main(int argc, char **argv)
{
    AUTO_REBUILD_SELF(argc, argv);

    TEST("char *_fmt_str(const char *fmt, ...)") {
        const char *expected = "Hello 1 2 3\n";
        assert(strcmp(expected, _fmt_str("Hello %d %d %d\n", 1, 2, 3)) == 0);
        LOG(NOBU_SUCCESS, "test passed: char *_fmt_str(const char *fmt, ...)\n", NULL);
    }

    TEST("char *_strconcat(const char *str, ...)") {
        const char *expected = "Hello, World!\n";
        assert(strcmp(expected, CONCAT("Hello, ", "World", "!", "\n")) == 0);
        LOG(NOBU_SUCCESS, "test passed: char *_strconcat(const char *str, ...)\n", NULL);
    }

    return 0;
}
