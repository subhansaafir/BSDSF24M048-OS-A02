#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

static void permissions(mode_t mode, char *p)
{
    p[0] = S_ISDIR(mode) ? 'd' :
           S_ISLNK(mode) ? 'l' :
           S_ISCHR(mode) ? 'c' :
           S_ISBLK(mode) ? 'b' :
           S_ISFIFO(mode) ? 'p' :
           S_ISSOCK(mode) ? 's' : '-';

    p[1] = mode & S_IRUSR ? 'r' : '-';
    p[2] = mode & S_IWUSR ? 'w' : '-';
    p[3] = mode & S_ISUID ? (mode & S_IXUSR ? 's' : 'S') :
           (mode & S_IXUSR ? 'x' : '-');
    p[4] = mode & S_IRGRP ? 'r' : '-';
    p[5] = mode & S_IWGRP ? 'w' : '-';
    p[6] = mode & S_ISGID ? (mode & S_IXGRP ? 's' : 'S') :
           (mode & S_IXGRP ? 'x' : '-');
    p[7] = mode & S_IROTH ? 'r' : '-';
    p[8] = mode & S_IWOTH ? 'w' : '-';
    p[9] = mode & S_ISVTX ? (mode & S_IXOTH ? 't' : 'T') :
           (mode & S_IXOTH ? 'x' : '-');
    p[10] = '\0';
}

static void long_listing(const char *dir, const char *name)
{
    char path[4096];
    struct stat st;
    char perm[11];
    char timebuf[32];
    struct passwd *pw;
    struct group *gr;
    struct tm *tm_info;
    const char *owner, *group;

    if (strcmp(dir, ".") == 0)
        snprintf(path, sizeof(path), "%s", name);
    else
        snprintf(path, sizeof(path), "%s/%s", dir, name);

    if (lstat(path, &st) == -1) {
        perror(path);
        return;
    }

    permissions(st.st_mode, perm);

    pw = getpwuid(st.st_uid);
    gr = getgrgid(st.st_gid);
    owner = pw ? pw->pw_name : "?";
    group = gr ? gr->gr_name : "?";

    tm_info = localtime(&st.st_mtime);
    if (tm_info)
        strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm_info);
    else
        strcpy(timebuf, "??? ?? ??:??");

    printf("%s %3lu %-8s %-8s %8lld %s %s\n",
           perm, (unsigned long)st.st_nlink,
           owner, group, (long long)st.st_size,
           timebuf, name);
}

static void do_ls(const char *dir, int long_format)
{
    DIR *dp = opendir(dir);
    struct dirent *entry;

    if (!dp) {
        perror(dir);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (long_format)
            long_listing(dir, entry->d_name);
        else
            printf("%s\n", entry->d_name);
    }

    closedir(dp);
}

int main(int argc, char *argv[])
{
    int opt;
    int long_format = 0;

    while ((opt = getopt(argc, argv, "l")) != -1) {
        if (opt == 'l')
            long_format = 1;
        else {
            fprintf(stderr, "Usage: %s [-l] [directory ...]\n",
                    argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (optind == argc) {
        do_ls(".", long_format);
    } else {
        for (int i = optind; i < argc; i++)
            do_ls(argv[i], long_format);
    }

    return EXIT_SUCCESS;
}
