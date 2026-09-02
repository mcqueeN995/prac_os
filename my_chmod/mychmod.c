#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int is_numeric_mode(const char *mode) {
    int len = strlen(mode);
    if (len < 1 || len > 4) return 0;
    for (int i = 0; mode[i]; i++) {
        if (mode[i] < '0' || mode[i] > '7') return 0;
    }
    return 1;
}

mode_t parse_numeric_mode(const char *mode) {
    mode_t result = 0;
    int len = strlen(mode);

    if (len == 4) {
        result |= ((mode_t)(mode[0] - '0')) << 9;
        result |= ((mode_t)(mode[1] - '0')) << 6;
        result |= ((mode_t)(mode[2] - '0')) << 3;
        result |= ((mode_t)(mode[3] - '0'));
    } else {
        result |= ((mode_t)(mode[0] - '0')) << 6;
        result |= ((mode_t)(mode[1] - '0')) << 3;
        result |= ((mode_t)(mode[2] - '0'));
    }

    return result;
}

int parse_symbolic_op(const char *op_str, mode_t *current_mode, mode_t st_mode_full) {
    const char *p = op_str;

    mode_t who_mask = 0;
    int has_who = 0;
    while (*p == 'u' || *p == 'g' || *p == 'o' || *p == 'a') {
        has_who = 1;
        switch (*p) {
            case 'u': who_mask |= S_IRWXU; break;
            case 'g': who_mask |= S_IRWXG; break;
            case 'o': who_mask |= S_IRWXO; break;
            case 'a': who_mask = S_IRWXU | S_IRWXG | S_IRWXO; break;
        }
        p++;
    }

    if (!has_who) {
        who_mask = S_IRWXU | S_IRWXG | S_IRWXO;
    }

    char op = *p++;
    if (op != '+' && op != '-' && op != '=') {
        fprintf(stderr, "mychmod: invalid operator '%c'\n", op);
        return -1;
    }

    mode_t perms = 0;
    int set_setuid = 0, set_setgid = 0, set_sticky = 0;

    while (*p) {
        switch (*p) {
            case 'r': perms |= 04; break;
            case 'w': perms |= 02; break;
            case 'x': perms |= 01; break;
            case 'X':
                if (S_ISDIR(st_mode_full) || (st_mode_full & (S_IXUSR | S_IXGRP | S_IXOTH)))
                    perms |= 01;
                break;
            case 's':
                if (who_mask & S_IRWXU) set_setuid = 1;
                if (who_mask & S_IRWXG) set_setgid = 1;
                break;
            case 't':
                set_sticky = 1;
                break;
            default:
                fprintf(stderr, "mychmod: invalid permission '%c'\n", *p);
                return -1;
        }
        p++;
    }

    mode_t full_perms = 0;
    if (who_mask & S_IRWXU) full_perms |= (perms << 6);
    if (who_mask & S_IRWXG) full_perms |= (perms << 3);
    if (who_mask & S_IRWXO) full_perms |= perms;

    switch (op) {
        case '+':
            *current_mode |= full_perms;
            break;
        case '-':
            *current_mode &= ~full_perms;
            break;
        case '=':
            *current_mode &= ~who_mask;
            *current_mode |= full_perms;
            break;
    }

    if (set_setuid) *current_mode |= S_ISUID;
    if (set_setgid) *current_mode |= S_ISGID;
    if (set_sticky) *current_mode |= S_ISVTX;

    return 0;
}

int parse_symbolic_mode(const char *mode, mode_t *current_mode, mode_t st_mode_full) {
    char *mode_copy = strdup(mode);
    if (!mode_copy) return -1;

    char *token = strtok(mode_copy, ",");
    while (token) {
        if (parse_symbolic_op(token, current_mode, st_mode_full) != 0) {
            free(mode_copy);
            return -1;
        }
        token = strtok(NULL, ",");
    }

    free(mode_copy);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s mode file...\n", argv[0]);
        return 1;
    }

    const char *mode_str = argv[1];
    int numeric = is_numeric_mode(mode_str);
    int exit_code = 0;

    for (int i = 2; i < argc; i++) {
        struct stat st;
        if (lstat(argv[i], &st) == -1) {
            perror(argv[i]);
            exit_code = 1;
            continue;
        }

        mode_t new_mode = st.st_mode & 07777;

        if (numeric) {
            new_mode = parse_numeric_mode(mode_str);
        } else {
            if (parse_symbolic_mode(mode_str, &new_mode, st.st_mode) != 0) {
                fprintf(stderr, "mychmod: invalid mode '%s'\n", mode_str);
                return 1;
            }
        }

        if (chmod(argv[i], new_mode) == -1) {
            perror(argv[i]);
            exit_code = 1;
        }
    }

    return exit_code;
}