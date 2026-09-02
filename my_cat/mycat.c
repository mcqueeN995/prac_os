#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

void process_file(FILE *stream, int flag_n, int flag_b, int flag_E) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 1;

    while ((read = getline(&line, &len, stream)) != -1) {
        int is_empty = (read == 1 && line[0] == '\n');

        int should_number = 0;
        if (flag_b) {
            should_number = !is_empty;
        } else if (flag_n) {
            should_number = 1;
        }

        if (should_number) {
            printf("%6d\t", line_number++);
        }

        int has_newline = (read > 0 && line[read - 1] == '\n');

        if (has_newline) {
            line[read - 1] = '\0';
        }

        if (flag_E) {
            printf("%s$", line);
        } else {
            printf("%s", line);
        }

        printf("\n");

        if (has_newline) {
            line[read - 1] = '\n';
        }
    }

    free(line);
}

int main(int argc, char *argv[]) {
    int opt;
    int flag_n = 0;
    int flag_b = 0;
    int flag_E = 0;

    static struct option long_options[] = {
        {"number",          no_argument, 0, 'n'},
        {"number-nonblank", no_argument, 0, 'b'},
        {"show-ends",       no_argument, 0, 'E'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "nbE", long_options, NULL)) != -1) {
        switch (opt) {
            case 'n': flag_n = 1; break;
            case 'b': flag_b = 1; break;
            case 'E': flag_E = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-nbE] [--number] [--number-nonblank] [--show-ends] [file ...]\n", argv[0]);
                return 1;
        }
    }

    if (optind == argc) {
        process_file(stdin, flag_n, flag_b, flag_E);
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                perror("Ошибка открытия файла");
                continue;
            }

            process_file(file, flag_n, flag_b, flag_E);

            fclose(file);
        }
    }

    return 0;
}