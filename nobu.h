#ifndef NOBU_H_
#define NOBU_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define CMD_INIT(...)                                               \
    do {                                                            \
        Cmd_Array cmds = _cmd_array_make(__VA_ARGS__, NULL);        \
        cmd_exec(&cmds);                                            \
        _cmd_array_free(&cmds);                                     \
    } while (0)                                                     \

typedef struct {
    const char **cmds;
    size_t count;
} Cmd_Array;

Cmd_Array _cmd_array_make(const char *cmd, ...);
void _cmd_array_free(Cmd_Array *cmd_arr);

Cmd_Array _cmd_array_make(const char *cmd, ...)
{
    Cmd_Array res = {0};
    if (cmd == NULL) return res;
    res.count = 1;

    const char *next; // store the arg from va_arg
    va_list args;
    va_start(args, cmd);
    while ((next = va_arg(args, const char *)) != NULL)
        res.count++;
    va_end(args);

    res.cmds = (const char **)malloc((res.count + 1) * sizeof(res.cmds[0])); // ALlocate except for the null terminator
    if (res.cmds == NULL) {
        fprintf(stderr, "[ERROR]: failed to allocate memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    res.count = 0;
    res.cmds[res.count++] = strdup(cmd);

    va_start(args, cmd);
    while ((next = va_arg(args, const char *)) != NULL)
        res.cmds[res.count++] = strdup(next);
    va_end(args);

    return res;
}

void _cmd_array_free(Cmd_Array *cmd_arr)
{
    for (size_t i = 0; i < cmd_arr->count; i++)
        free((char *)cmd_arr->cmds[i]);
    free(cmd_arr->cmds);
    cmd_arr->cmds = NULL;
    cmd_arr->count = 0;
}

int cmd_exec(Cmd_Array *cmd_arr);
int _is_source_modified(const char *source_path, const char *binary_path);
int get_time(const char *pathname);

int cmd_exec(Cmd_Array *cmd_arr)
{
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "[ERROR]: could not fork child process: %s\n", strerror(errno));
        return -1;
    }

    if (pid == 0) {
        execvp(cmd_arr->cmds[0], (char * const *)cmd_arr->cmds); // This also execute the null terminator due to appending null from args
        fprintf(stderr, "[ERROR]: could not execute child process: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }  else {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0)
            fprintf(stderr, "[ERROR]: waitpid failed for process %d: %s\n", pid, strerror(errno));

        if (WIFEXITED(wstatus))
            return WEXITSTATUS(wstatus);
        else
            return -1;
    }
}

int get_time(const char *pathname)
{
    struct stat statbuf = {0};

    if (stat(pathname, &statbuf) < 0) {
        fprintf(stderr, "ERROR: could not stat %s: %s\n", pathname, strerror(errno));
        exit(1);
    }

    return statbuf.st_mtimespec.tv_sec;
}

int _is_source_modified(const char *source_path, const char *binary_path)
{
    struct stat statbuf = {0};

    int source_time = get_time(source_path);
    int binary_time = get_time(binary_path);

    return source_time > binary_time;
}

#endif // NOBU_H_
