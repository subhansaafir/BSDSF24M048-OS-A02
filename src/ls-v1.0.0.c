#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>

/* ANSI terminal colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_PINK    "\033[1;35m"
#define COLOR_REVERSE "\033[7m"

static int compare_names(const void *a, const void *b)
{
    const char *name_a = *(const char * const *)a;
    const char *name_b = *(const char * const *)b;

    return strcmp(name_a, name_b);
}

static void permissions(mode_t mode, char *p)
{
    p[0] = S_ISDIR(mode) ? 'd' :
           S_ISLNK(mode) ? 'l' :
           S_ISCHR(mode) ? 'c' :
           S_ISBLK(mode) ? 'b' :
           S_ISFIFO(mode) ? 'p' :
           S_ISSOCK(mode) ? 's' : '-';

    p[1] = (mode & S_IRUSR) ? 'r' : '-';
    p[2] = (mode & S_IWUSR) ? 'w' : '-';
    p[3] = (mode & S_ISUID) ? ((mode & S_IXUSR) ? 's' : 'S') :
           ((mode & S_IXUSR) ? 'x' : '-');

    p[4] = (mode & S_IRGRP) ? 'r' : '-';
    p[5] = (mode & S_IWGRP) ? 'w' : '-';
    p[6] = (mode & S_ISGID) ? ((mode & S_IXGRP) ? 's' : 'S') :
           ((mode & S_IXGRP) ? 'x' : '-');

    p[7] = (mode & S_IROTH) ? 'r' : '-';
    p[8] = (mode & S_IWOTH) ? 'w' : '-';
    p[9] = (mode & S_ISVTX) ? ((mode & S_IXOTH) ? 't' : 'T') :
           ((mode & S_IXOTH) ? 'x' : '-');

    p[10] = '\0';
}

static int has_suffix(const char *name, const char *suffix)
{
    size_t name_len = strlen(name);
    size_t suffix_len = strlen(suffix);

    if (name_len < suffix_len)
        return 0;

    return strcmp(name + name_len - suffix_len, suffix) == 0;
}

/*
 * Print a filename in the appropriate color.
 * path is used to inspect the actual filesystem entry.
 * name is the filename shown to the user.
 */
static void print_colored_name(const char *path, const char *name)
{
    struct stat st;
    const char *color = "";
    int use_color = isatty(STDOUT_FILENO);

    if (!use_color) {
        printf("%s", name);
        return;
    }

    if (lstat(path, &st) == -1) {
        printf("%s", name);
        return;
    }

    /* Check links and special types before permission-based coloring. */
    if (S_ISLNK(st.st_mode)) {
        color = COLOR_PINK;
    } else if (S_ISDIR(st.st_mode)) {
        color = COLOR_BLUE;
    } else if (S_ISCHR(st.st_mode) ||
               S_ISBLK(st.st_mode) ||
               S_ISFIFO(st.st_mode) ||
               S_ISSOCK(st.st_mode)) {
        color = COLOR_REVERSE;
    } else if (has_suffix(name, ".tar") ||
               has_suffix(name, ".gz") ||
               has_suffix(name, ".zip")) {
        color = COLOR_RED;
    } else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        color = COLOR_GREEN;
    }

    printf("%s%s%s", color, name, COLOR_RESET);
}

static void long_listing(const char *dir, const char *name)
{
    char path[PATH_MAX];
    char perm[11];
    char timebuf[32];
    struct stat st;

    int written = snprintf(path, sizeof(path), "%s/%s", dir, name);
    if (written < 0 || (size_t)written >= sizeof(path)) {
        fprintf(stderr, "Path is too long: %s/%s\n", dir, name);
        return;
    }

    if (lstat(path, &st) == -1) {
        perror(path);
        return;
    }

    permissions(st.st_mode, perm);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    const char *t = ctime(&st.st_mtime);

    if (t != NULL)
        snprintf(timebuf, sizeof(timebuf), "%.24s", t);
    else
        strcpy(timebuf, "unknown time");

    printf("%s %3lu %-8s %-8s %8lld %s ",
           perm,
           (unsigned long)st.st_nlink,
           pw ? pw->pw_name : "?",
           gr ? gr->gr_name : "?",
           (long long)st.st_size,
           timebuf);

    print_colored_name(path, name);
    putchar('\n');
}

static int terminal_width(void)
{
    struct winsize ws;

    if (isatty(STDOUT_FILENO) &&
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0) {
        return ws.ws_col;
    }

    return 80;
}

static void list_directory(const char *dir, int long_format, int horizontal)
{
    DIR *dp = opendir(dir);
    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t maxlen = 0;

    if (dp == NULL) {
        perror(dir);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        char **tmp;
        size_t len;

        /* Match the usual ls default: hide names beginning with '.'. */
        if (entry->d_name[0] == '.')
            continue;

        if (count == capacity) {
            size_t new_capacity = capacity ? capacity * 2 : 16;

            tmp = realloc(names, new_capacity * sizeof(*names));
            if (tmp == NULL) {
                perror("realloc");
                break;
            }

            names = tmp;
            capacity = new_capacity;
        }

        names[count] = strdup(entry->d_name);
        if (names[count] == NULL) {
            perror("strdup");
            break;
        }

        len = strlen(names[count]);
        if (len > maxlen)
            maxlen = len;

        count++;
    }

    closedir(dp);

    if (count > 1)
        qsort(names, count, sizeof(*names), compare_names);

    if (long_format) {
        for (size_t i = 0; i < count; i++)
            long_listing(dir, names[i]);
    } else if (count > 0) {
        size_t cell_width = maxlen + 2;
        size_t cols = (size_t)terminal_width() / cell_width;

        if (cols == 0)
            cols = 1;
        if (cols > count)
            cols = count;

        if (horizontal) {
            /* Feature 4: print across each row first. */
            for (size_t i = 0; i < count; i++) {
                char path[PATH_MAX];
                int written = snprintf(path, sizeof(path), "%s/%s",
                                      dir, names[i]);

                if (written < 0 || (size_t)written >= sizeof(path)) {
                    printf("%-*s", (int)cell_width, names[i]);
                } else {
                    print_colored_name(path, names[i]);
                    if (i + 1 < count)
                        printf("%*s", (int)(cell_width - strlen(names[i])), "");
                }

                if ((i + 1) % cols == 0 || i + 1 == count)
                    putchar('\n');
            }
        } else {
            /* Feature 3: fill down columns, then across. */
            size_t rows = (count + cols - 1) / cols;

            for (size_t r = 0; r < rows; r++) {
                for (size_t c = 0; c < cols; c++) {
                    size_t i = c * rows + r;

                    if (i < count) {
                        char path[PATH_MAX];
                        int written = snprintf(path, sizeof(path), "%s/%s",
                                              dir, names[i]);

                        if (written < 0 ||
                            (size_t)written >= sizeof(path)) {
                            printf("%-*s", (int)cell_width, names[i]);
                        } else {
                            print_colored_name(path, names[i]);

                            if (c + 1 < cols && i + rows < count)
                                printf("%*s",
                                       (int)(cell_width - strlen(names[i])),
                                       "");
                        }
                    }
                }

                putchar('\n');
            }
        }
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

int main(int argc, char *argv[])
{
    int opt;
    int long_format = 0;
    int horizontal = 0;

    while ((opt = getopt(argc, argv, "lx")) != -1) {
        if (opt == 'l') {
            long_format = 1;
        } else if (opt == 'x') {
            horizontal = 1;
        } else {
            fprintf(stderr, "Usage: %s [-l] [-x] [directory ...]\n",
                    argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (optind == argc) {
        list_directory(".", long_format, horizontal);
    } else {
        for (int i = optind; i < argc; i++)
            list_directory(argv[i], long_format, horizontal);
    }

    return EXIT_SUCCESS;
}
