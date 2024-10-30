#include <assert.h>
#include <stdio.h>

#include "nobu.h"

int main(int argc, char **argv)
{
    const char *source_path = __FILE__;
    assert(argc >= 1);
    const char *binary_path = argv[0];

    if (_is_source_modified(source_path, binary_path)) {
        CMD_INIT("cc", "-o", binary_path, source_path);
        CMD_INIT(binary_path);
        exit(0);
    }

    printf("Hello, World!\n");

    return 0;
}
