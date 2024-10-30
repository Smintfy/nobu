#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./nobu.h"

int get_time(const char *pathname)
{
    struct stat statbuf = {0};

    if (stat(pathname, &statbuf) < 0) {
        fprintf(stderr, "ERROR: could not stat %s: %s\n", pathname, strerror(errno));
        exit(1);
    }

    return statbuf.st_mtimespec.tv_sec;
}

int main(int argc, char **argv)
{
    const char *source_path = __FILE__;
    int source_time = get_time(source_path);
    assert(argc >= 1);
    const char *binary_path = argv[0];
    int binary_time = get_time(binary_path);

    if (source_time > binary_time) {
        printf("TODO: Rebuild\n"); // Only prints when this file is modified
    } else {
        printf("UP TO DATE\n");
    }

    return 0;
}
