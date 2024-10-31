#include <assert.h>
#include <stdio.h>

#include "nobu.h"

int main(int argc, char **argv)
{
    AUTO_REBUILD_SELF(argc, argv);

    printf("Hello, World!\n");

    return 0;
}
