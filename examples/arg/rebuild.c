#include "../../nobu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIN_PATH(fname) CONCAT("./", fname)
#define SOURCE_PATH(fname) CONCAT(fname, ".c")

int main(int argc, char **argv)
{
    AUTO_REBUILD_SELF(argc, argv);

    /* needs to be rebuild once before using the args */
    if (argc > 1) {
        LOG(NOBU_INFO, "compiling %s\n", BIN_PATH(argv[1]));

        /* ./rebuild hello will build hello.c */
        /* ./rebuild hello_world will build hello_world.c */
        CMD_INIT(CC, "-o", BIN_PATH(argv[1]), SOURCE_PATH(argv[1]));
        CMD_INIT(BIN_PATH(argv[1]));
        exit(0);
    };

    printf("Hello World!\n");

    return 0;
}
