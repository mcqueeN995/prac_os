#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void search_in_stream(FILE *stream, const char *pattern) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stream)) != -1) {
        if (strstr(line, pattern) != NULL) {
            printf("%s", line);
        }
    }

    free(line);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s pattern [file ...]\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];

    if (argc == 2) {
        search_in_stream(stdin, pattern);
    } else {
        for (int i = 2; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                perror("Ошибка открытия файла");
                continue;
            }

            search_in_stream(file, pattern);

            fclose(file);
        }
    }

    return 0;
}