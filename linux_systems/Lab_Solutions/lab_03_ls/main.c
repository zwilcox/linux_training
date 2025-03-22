#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


int print_file_info(char* file_path, char* file_name);
int print_dir_info(char* dir_path);

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return print_dir_info(argv[1]);
    }
    return print_dir_info(".");
}

int print_dir_info(char* dir_path) {
    DIR* dir_p;
    char file_path[PATH_MAX];
    struct dirent* entry_p;
    dir_p = opendir(dir_path);
    if (dir_p == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((entry_p = readdir(dir_p)) != NULL) {
        if (strcmp(entry_p->d_name, ".") == 0 || 
            strcmp(entry_p->d_name, "..") == 0) {
            continue;
        }
        snprintf(file_path, PATH_MAX, "%s/%s", dir_path, entry_p->d_name);
        (void)print_file_info(file_path, entry_p->d_name);
    }
    if (closedir(dir_p) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int print_file_info(char* file_path, char* file_name) {
    struct stat file_info;
    struct passwd* pw;
    struct group* gr;
    struct tm* p_time;
    char time_buf[256];
    char user_buf[256];
    char group_buf[256];

    /* get file information */
    if (stat(file_path, &file_info) != 0) {
        fprintf(stderr, "stat failed on %s: %s\n", file_path, strerror(errno));
        return 1;
    }

    /* get user name from UID if possible */
    pw = getpwuid(file_info.st_uid);
    if (pw == NULL) {
        snprintf(user_buf, sizeof(user_buf), "%-8d", file_info.st_uid);
    }
    else {
        snprintf(user_buf, sizeof(user_buf), "%-8.8s", pw->pw_name);
    }

    /* get group name from GID if possible */
    gr = getgrgid(file_info.st_gid);
    if (gr == NULL) {
        snprintf(group_buf, sizeof(group_buf), "%-8d", file_info.st_gid);
    }
    else {
        snprintf(group_buf, sizeof(group_buf), "%-8.8s", gr->gr_name);
    }

    printf((S_ISDIR(file_info.st_mode)) ? "d" : "-");
    printf((file_info.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_info.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_info.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_info.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_info.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_info.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_info.st_mode & S_IROTH) ? "r" : "-");
    printf((file_info.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_info.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %jd", file_info.st_nlink);
    printf(" %s %s", user_buf, group_buf);
    printf(" %5jd", file_info.st_size);
    p_time = localtime(&file_info.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", p_time);
    printf(" %s", time_buf);
    printf(" %s\n", file_name);

    if (S_ISDIR(file_info.st_mode)) {
        print_dir_info(file_path);
    }

    return 0;
}

