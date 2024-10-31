#ifndef NOBU_H_
#define NOBU_H_

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define CC "cc"

enum Log_Type {
    NOBU_INFO,
    NOBU_ERROR,
    NOBU_SUCCESS,
};

void _fmt_print_log(enum Log_Type type, char *message);
char *_fmt_str(const char *fmt, ...);

#define LOG(type, message, spec) _fmt_print_log(type, _fmt_str(message, spec))

/* concatenate strings */
#define CONCAT(...) _strconcat(__VA_ARGS__, NULL)

/* macro for executing commands */
#define CMD_INIT(...)                                           \
    do {                                                        \
        Cmd_Array cmds = _cmd_array_init(__VA_ARGS__, NULL);    \
        cmd_exec(&cmds);                                        \
        _cmd_array_free(&cmds);                                 \
    } while (0)                                                 \

/* macro for rebuilding a program only when the source updated  */
#define AUTO_REBUILD_SELF(argc, argv)                           \
    do {                                                        \
        const char *source_path = __FILE__;                     \
        assert(argc >= 1);                                      \
        const char *binary_path = argv[0];                      \
        if (_is_source_modified(source_path, binary_path)) {    \
            LOG(NOBU_INFO, "rebuilding %s\n", source_path);     \
            CMD_INIT("cc", "-o", binary_path, source_path);     \
            CMD_INIT(binary_path);                              \
            exit(0);                                            \
        } else {                                                \
            LOG(NOBU_INFO, "%s up to date\n", source_path);     \
        }                                                       \
    } while (0)

typedef struct {
    const char **cmds;
    size_t count;
} Cmd_Array;

Cmd_Array _cmd_array_init(const char *cmd, ...);
void _cmd_array_free(Cmd_Array *cmd_arr);

Cmd_Array _cmd_array_init(const char *cmd, ...)
{
    Cmd_Array res = {0};
    if (cmd == NULL) return res;
    res.count = 1;

    const char *next; /* store the arg from va_arg */
    va_list args;
    va_start(args, cmd); /* iterate through each args */
    while ((next = va_arg(args, const char *)) != NULL)
        res.count += 1;
    va_end(args);

    res.cmds = (const char **)malloc((res.count + 1) * sizeof(res.cmds[0])); /* allocate with extra 8 bytes for the NULL terminator */
    if (res.cmds == NULL) {
        LOG(NOBU_ERROR, "failed to allocate memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    res.count = 0;
    res.cmds[res.count++] = strdup(cmd); /* initialize the first cmd */

    va_start(args, cmd);
    while ((next = va_arg(args, const char *)) != NULL) /* iterate through each args */
        res.cmds[res.count++] = strdup(next); /* copy each args to the res array */
    va_end(args);

    return res;
}

void _cmd_array_free(Cmd_Array *cmd_arr)
{
    for (size_t i = 0; i < cmd_arr->count; i++)
        free((char *)cmd_arr->cmds[i]);
    free(cmd_arr->cmds);
    cmd_arr->cmds = NULL; /* prevent dangling pointer */
    cmd_arr->count = 0;
}

int cmd_exec(Cmd_Array *cmd_arr);
char *_strconcat(const char *str, ...);
int _is_source_modified(const char *source_path, const char *binary_path);
int _get_time(const char *pathname);
void _fmt_print_log(enum Log_Type type, char *message);

int cmd_exec(Cmd_Array *cmd_arr)
{
    pid_t pid = fork();

    if (pid < 0) {
        LOG(NOBU_ERROR, "could not fork child process: %s\n", strerror(errno));
        return -1;
    }

    if (pid == 0) {
        execvp(cmd_arr->cmds[0], (char * const *)cmd_arr->cmds);
        LOG(NOBU_ERROR, "could not execute child process: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }  else {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0)
            LOG(NOBU_ERROR, "waitpid failed for process %d: %s\n", strerror(errno));

        if (WIFEXITED(wstatus))
            return WEXITSTATUS(wstatus); /* process exit normally */
        else
            return -1;
    }
}

int _get_time(const char *pathname)
{
    struct stat statbuf = {0};

    if (stat(pathname, &statbuf) < 0) {
        LOG(NOBU_ERROR, "could not stat %s: %s\n", strerror(errno));
        exit(1);
    }

    /* #define st_mtime st_mtimespec.tv_sec (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
    return statbuf.st_mtime; /* time of last modification */
}

int _is_source_modified(const char *source_path, const char *binary_path)
{
    struct stat statbuf = {0};

    int source_time = _get_time(source_path);
    int binary_time = _get_time(binary_path);

    return source_time > binary_time;
}

char *_strconcat(const char *str, ...)
{
    size_t len = strlen(str); /* initial length */
    const char *next;

    va_list args;
    va_start(args, str);
    while ((next = va_arg(args, const char *)) != NULL)
        len += strlen(next);
    va_end(args);

    char *res = (char *)malloc((len + 1) * sizeof(char));
    if (res == NULL) {
        LOG(NOBU_ERROR, "failed to allocate memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    strcpy(res, str); /* res <- str */

    va_start(args, str);
    while ((next = va_arg(args, const char *)) != NULL) {
        strcat(res, next); /* iterate through each arg and appending it to res */
    }
    va_end(args);

    return res;
}

char *_fmt_str(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len < 0) return NULL;

    char *res = (char *)malloc(len + 1);
    if (!res) return NULL;

    va_start(args, fmt);
    vsnprintf(res, len + 1, fmt, args);
    va_end(args);

    return res;
}

void _fmt_print_log(enum Log_Type type, char *message)
{
    switch (type) {
        case NOBU_INFO:
            printf("[INFO]: %s", message);
            break;
        case NOBU_ERROR:
            fprintf(stderr, "[ERROR]: %s", message);
            break;
        case NOBU_SUCCESS:
            fprintf(stderr, "[SUCCESS]: %s", message);
            break;
        default:
            printf("Unknown Log_Type: %d", type);
    }
}

#endif // NOBU_H_
