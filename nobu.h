#ifndef NOBU_H_
#define NOBU_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
Cmd_Array _cmd_array_append(Cmd_Array *cmd_arr, const char *cmd);
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

    res.cmds = (const char **)malloc((res.count) * sizeof(res.cmds[0])); // ALlocate except for the null terminator
    if (res.cmds == NULL) {
        perror("Failed to allocate memory");
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

Cmd_Array _cmd_array_append(Cmd_Array *cmd_arr, const char *cmd)
{
    Cmd_Array res = {
        .count = cmd_arr->count + 1
    };
    res.cmds = (const char **)malloc(res.count * sizeof(res.cmds[0]));
    memcpy(res.cmds, cmd_arr->cmds, cmd_arr->count * sizeof(res.cmds[0]));
    res.cmds[cmd_arr->count] = strdup(cmd);
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
int _is_source_modified(char *source_path, char *binary_path);

int cmd_exec(Cmd_Array *cmd_arr)
{
    pid_t pid = fork();

    // TODO: better error
    if (pid < 0) {
        perror("Failed to fork\n");
        return -1;
    }

    if (pid == 0) {
        Cmd_Array args = _cmd_array_append(cmd_arr, NULL);

        // printf("Executing command: %s\n", args.cmds[0]);
        // for (size_t i = 0; i < args.count; i++)
        //     printf("Arg[%zu]: %s\n", i, args.cmds[i]);

        execvp(args.cmds[0], (char * const *)args.cmds); // This also execute the null terminator due to appending null from args
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }  else {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

// int _is_source_modified(char *source_path, char *binary_path)
// {
//     struct stat statbuf
// }

#endif // NOBU_H_
