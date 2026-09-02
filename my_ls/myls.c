#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_CYAN    "\033[1;36m"

typedef struct {
    char *name;
    char *full_path;
    struct stat st;
} file_info;

void mode_to_string(mode_t mode, char *buf) {
    buf[0] = S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' :
             S_ISCHR(mode) ? 'c' : S_ISBLK(mode) ? 'b' :
             S_ISFIFO(mode) ? 'p' : S_ISSOCK(mode) ? 's' : '-';
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
}

const char* get_color(const file_info *fi) {
    if (S_ISDIR(fi->st.st_mode)) return COLOR_BLUE;
    if (S_ISLNK(fi->st.st_mode)) return COLOR_CYAN;
    if (fi->st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return COLOR_GREEN;
    return COLOR_RESET;
}

void print_long_format(const file_info *fi, int max_nlink, int max_owner, int max_group, int max_size) {
    char mode_str[12];
    mode_to_string(fi->st.st_mode, mode_str);

    struct passwd *pw = getpwuid(fi->st.st_uid);
    struct group *gr = getgrgid(fi->st.st_gid);

    char time_buf[64];
    struct tm *tm = localtime(&fi->st.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%b %e %H:%M", tm);

    const char *color = get_color(fi);

    printf("%s %*ld %-*s %-*s %*ld %s %s%s%s",
           mode_str,
           max_nlink, (long)fi->st.st_nlink,
           max_owner, pw ? pw->pw_name : "unknown",
           max_group, gr ? gr->gr_name : "unknown",
           max_size, (long)fi->st.st_size,
           time_buf,
           color,
           fi->name,
           COLOR_RESET);

    if (S_ISLNK(fi->st.st_mode)) {
        char link_target[1024];
        ssize_t len = readlink(fi->full_path, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }

    printf("\n");
}

int custom_alphasort(const struct dirent **a, const struct dirent **b) {
    return strcoll((*a)->d_name, (*b)->d_name);
}

void process_directory(const char *path, int flag_a, int flag_l) {
    struct dirent **namelist;
    int n;

    n = scandir(path, &namelist, NULL, custom_alphasort);
    if (n < 0) {
        perror(path);
        return;
    }

    file_info *files = malloc(n * sizeof(file_info));
    int count = 0;
    long total_blocks = 0;

    for (int i = 0; i < n; i++) {
        if (!flag_a && (strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0)) {
            free(namelist[i]);
            continue;
        }

        char full_path[4096];
        if (strcmp(path, ".") == 0) {
            snprintf(full_path, sizeof(full_path), "%s", namelist[i]->d_name);
        } else {
            snprintf(full_path, sizeof(full_path), "%s/%s", path, namelist[i]->d_name);
        }

        if (lstat(full_path, &files[count].st) == -1) {
            free(namelist[i]);
            continue;
        }

        files[count].name = strdup(namelist[i]->d_name);
        files[count].full_path = strdup(full_path);
        total_blocks += files[count].st.st_blocks;
        count++;

        free(namelist[i]);
    }
    free(namelist);

    int max_nlink = 1, max_owner = 1, max_group = 1, max_size = 1;
    if (flag_l) {
        for (int i = 0; i < count; i++) {
            int nlink_len = snprintf(NULL, 0, "%ld", (long)files[i].st.st_nlink);
            int size_len = snprintf(NULL, 0, "%ld", (long)files[i].st.st_size);

            struct passwd *pw = getpwuid(files[i].st.st_uid);
            struct group *gr = getgrgid(files[i].st.st_gid);
            int owner_len = strlen(pw ? pw->pw_name : "unknown");
            int group_len = strlen(gr ? gr->gr_name : "unknown");

            if (nlink_len > max_nlink) max_nlink = nlink_len;
            if (owner_len > max_owner) max_owner = owner_len;
            if (group_len > max_group) max_group = group_len;
            if (size_len > max_size) max_size = size_len;
        }
    }

    if (flag_l) {
        printf("итого %ld\n", total_blocks / 2);
        for (int i = 0; i < count; i++) {
            print_long_format(&files[i], max_nlink, max_owner, max_group, max_size);
            free(files[i].name);
            free(files[i].full_path);
        }
    } else {
        for (int i = 0; i < count; i++) {
            const char *color = get_color(&files[i]);
            printf("%s%s%s\n", color, files[i].name, COLOR_RESET);
            free(files[i].name);
            free(files[i].full_path);
        }
    }

    free(files);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");

    int opt;
    int flag_a = 0;
    int flag_l = 0;

    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
            case 'a': flag_a = 1; break;
            case 'l': flag_l = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-al] [directory ...]\n", argv[0]);
                return 1;
        }
    }

    if (optind == argc) {
        process_directory(".", flag_a, flag_l);
    } else {
        for (int i = optind; i < argc; i++) {
            struct stat st;
            if (lstat(argv[i], &st) == -1) {
                perror(argv[i]);
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                if (argc - optind > 1) {
                    printf("%s:\n", argv[i]);
                }
                process_directory(argv[i], flag_a, flag_l);
                if (i < argc - 1) printf("\n");
            } else {
                file_info fi;
                fi.name = argv[i];
                fi.full_path = argv[i];
                fi.st = st;
                if (flag_l) {
                    print_long_format(&fi, 1, 1, 1, 1);
                } else {
                    const char *color = get_color(&fi);
                    printf("%s%s%s\n", color, fi.name, COLOR_RESET);
                }
            }
        }
    }

    return 0;
}