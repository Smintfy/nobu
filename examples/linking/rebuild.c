#include "../../nobu.h"

/* compiler flags */
#define C_FLAGS "-Wall", "-Wextra"
/* link files or modules */
#define LIBS "add.c", "subtract.c"
void rebuild_project()
{
    const char *source_path = "main.c";
    const char *binary_path = "./main";

    LOG(NOBU_INFO, "rebuilding %s\n", source_path);
    CMD_INIT(CC, C_FLAGS, "-o", binary_path, source_path, LIBS);
    CMD_INIT(binary_path);
    exit(0);
}

int main(int argc, char **argv)
{
    AUTO_REBUILD_SELF(argc, argv); /* rebuild self (nobu.c) only when updated */
    rebuild_project(); /* rebuild and run main every single time ./nobu is executed */

    return 0;
}
