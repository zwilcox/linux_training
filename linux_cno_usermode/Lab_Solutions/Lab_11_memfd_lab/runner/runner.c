#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/inotify.h>
#include <linux/limits.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <sys/param.h>
#include <grp.h>

/* Just make this value smaller than the payload, 
 * but much larger than a decent-sized stager */
#define MAX_PROGRAM_SIZE	100000

#define RST  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YLW  "\x1B[33m"
#define BLU  "\x1B[34m"
#define BLD  "\x1B[1m"

typedef struct id_info {
	uid_t uid;
	gid_t gid;
	int num_groups;
	gid_t groups[NGROUPS_MAX];
} id_info_t;

typedef struct param {
	char path[PATH_MAX];
	int pid;
	int fd;
} param_t;

int drop_privs(uid_t new_uid, gid_t new_gid);
int drop_privs_temp(uid_t new_uid, gid_t new_gid, id_info_t* saved_id_info);
int restore_privs(id_info_t* saved_id_info);

int jail_include(char* jailpath, char* path);
int jail_remove(char* jailpath, char* path);
int fs_monitor(int notify_fd);
void* monitor_routine(void* param);
int printf_color(char* color, char* fmt, ...);

int main(int argc, char* argv[]) {
	struct stat st;
	char chroot_path[PATH_MAX];
	int status;
	int pid;
	int pipe_fd[2];
	char buffer[3];
	param_t param;
	pthread_t tid;
	if (argc < 2) {
		fprintf(stderr, "usage: %s path-to-binary\n", argv[0]);
		return EXIT_FAILURE;
	}

	getcwd(chroot_path, PATH_MAX);
	jail_include(chroot_path, "/proc");
	jail_include(chroot_path, "/etc");
	jail_include(chroot_path, "/lib64");

	if (pipe(pipe_fd) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	if (pid != 0) {
		close(pipe_fd[0]);
		strcpy(param.path, chroot_path);
		param.pid = pid;
		param.fd = pipe_fd[1];
		pthread_create(&tid, NULL, monitor_routine, (void*)&param);
		waitpid(pid, &status, 0);
		jail_remove(chroot_path, "/proc");
		jail_remove(chroot_path, "/etc");
		jail_remove(chroot_path, "/lib64");
		printf("Quitting\n");
		return EXIT_SUCCESS;
	}

	close(pipe_fd[1]);

	if (chroot(chroot_path) == -1) {
		perror("chroot");
	}

	if (chdir("/") == -1) {
		perror("chdir");
	}

	if (stat(argv[1], &st) == -1) {
		perror("stat");
	}
	printf("program size is %ld bytes\n", st.st_size);
	if (st.st_size > MAX_PROGRAM_SIZE) {
		printf_color("red", "FILE SIZE TOO LARGE\n");
		return EXIT_FAILURE;	
	}

	printf("Waiting for monitor\n");
	read(pipe_fd[0], buffer, sizeof(buffer));
	close(pipe_fd[0]);

	drop_privs(getuid(), getgid());

	printf("Launching program\n");
	if (execv(argv[1], argv+1) == -1) {
		perror("execv");
	}

	return EXIT_SUCCESS;
}

int jail_include(char* jailpath, char* path) {
	char path_in_jail[PATH_MAX];
	snprintf(path_in_jail, PATH_MAX-1, "%s%s", jailpath, path);
	//printf("mounting %s to %s\n", path, path_in_jail);
	mkdir(path_in_jail, 755);
	if (mount(path, path_in_jail, NULL, MS_BIND | MS_RDONLY, NULL) == -1) {
		perror("mount");
		return -1;
	}
	return 0;
}

int jail_remove(char* jailpath, char* path) {
	char path_in_jail[PATH_MAX];
	snprintf(path_in_jail, PATH_MAX-1, "%s%s", jailpath, path);
	//printf("unmounting %s\n", path_in_jail);
	if (umount2(path_in_jail, MNT_FORCE) == -1) {
		perror("umount");
	}
	if (rmdir(path_in_jail) == -1) {
		perror("unlink");
	}
	return 0;
}

int fs_monitor(int notify_fd) {
	ssize_t len;
	struct inotify_event* event;
	char buffer[sizeof(struct inotify_event) + NAME_MAX + 1]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));
	while (1) {
		len = read(notify_fd, buffer, sizeof(buffer));
		if (len == -1) {
			perror("read");
		}
		event = (struct inotify_event*)buffer;
		if (event->mask & IN_CREATE) {
			printf("%s has been created!\n", event->name);
			return 1;
		}
		if (event->mask & IN_MODIFY) {
			printf("%s has been modified!\n", event->name);
			return 1;
		}
	}
	return 0;
}

void* monitor_routine(void* arg) {
	int wd;
	param_t* param = (param_t*)arg;
	int notify_fd = inotify_init1(IN_CLOEXEC);
	if (notify_fd == -1) {
		perror("inotyify_init1");
		return NULL;
	}

	wd = inotify_add_watch(notify_fd, param->path, IN_CREATE | IN_MODIFY);
	if (wd == -1) {
		perror("inotify_add_watch");
		return NULL;
	}

	printf("Monitor active\n");
	write(param->fd, "GO", sizeof("GO"));
	close(param->fd);
	if (fs_monitor(notify_fd) == 1) {
		printf("Terminating process\n");
		printf_color("red", "YOU HAVE BEEN DETECTED\n");
		kill(param->pid, SIGKILL);
	}
	return NULL;
}

int printf_color(char* color, char* fmt, ...) {
    char* color_fmt;
    char* color_code;
    va_list args;
    int ret;

    if (strcmp(color, "red") == 0) {
        color_code = RED;
    }
    else if (strcmp(color, "green") == 0) {
        color_code = GRN;
    }
    else if (strcmp(color, "yellow") == 0) {
        color_code = YLW;
    }
    else {
        color_code = BLU;
    }

    color_fmt = malloc(strlen(fmt) + strlen(BLD) + strlen(color_code)
            + strlen(RST) + strlen(RST) + 1);
    if (color_fmt == NULL) {
        return 0;
    }

    sprintf(color_fmt, "%s%s%s%s%s", BLD, color_code, fmt, RST, RST);

    va_start(args, fmt);
    ret = vprintf(color_fmt, args);
    va_end(args);

    free(color_fmt);

    return ret;
}

int drop_privs(uid_t new_uid, gid_t new_gid) {
	uid_t current_uid;
	gid_t current_gid;
	
	current_uid = geteuid();
	current_gid = getegid();

	if (current_uid == 0) {
		if (setgroups(1, &new_gid) == -1) {
			perror("setgroups");
			exit(EXIT_FAILURE);
		}
	}

	if (new_gid != current_gid) {
		if (setregid(new_gid, new_gid) == -1) {
			perror("setregid");
			exit(EXIT_FAILURE);
		}
	}

	if (new_uid != current_uid) {
		if (setreuid(new_uid, new_uid) == -1) {
			perror("setreuid");
			exit(EXIT_FAILURE);
		}
	}

	/* verify success */
	if (new_gid != current_gid) {
		if (setegid(current_gid) != -1 || getegid() != new_gid) {
			fprintf(stderr, "Change of GID failed\n");
			exit(EXIT_FAILURE);
		}
	}

	if (new_uid != current_uid) {
		if (seteuid(current_uid) != -1 || geteuid() != new_uid) {
			fprintf(stderr, "Change of UID failed\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

int drop_privs_temp(uid_t new_uid, gid_t new_gid, id_info_t* saved_id_info) {
	uid_t current_uid;
	gid_t current_gid;
	
	current_uid = geteuid();
	current_gid = getegid();

	/* save off current IDs */
	saved_id_info->uid = current_uid;
	saved_id_info->gid = current_gid;
	saved_id_info->num_groups = getgroups(NGROUPS_MAX, saved_id_info->groups);
	if (saved_id_info->num_groups == -1) {
		perror("getgroups");
		exit(EXIT_FAILURE);
	}

	if (current_uid == 0) {
		if (setgroups(1, &new_gid) == -1) {
			perror("setgroups");
			exit(EXIT_FAILURE);
		}
	}

	if (new_gid != current_gid) {
		if (setregid(-1, new_gid) == -1) {
			perror("setregid");
			exit(EXIT_FAILURE);
		}
	}

	if (new_uid != current_uid) {
		if (setreuid(-1, new_uid) == -1) {
			perror("setreuid");
			exit(EXIT_FAILURE);
		}
	}

	/* verify success */
	if (new_gid != current_gid) {
		if (getegid() != new_gid) {
			fprintf(stderr, "Change of GID failed\n");
			exit(EXIT_FAILURE);
		}
	}
	if (new_uid != current_uid) {
		if (geteuid() != new_uid) {
			fprintf(stderr, "Change of UID failed\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

int restore_privs(id_info_t* saved_id_info) {
	if (geteuid() != saved_id_info->uid) {
		if (seteuid(saved_id_info->uid) == -1) {
			perror("seteuid");
			exit(EXIT_FAILURE);
		}
	}

	if (getegid() != saved_id_info->gid) {
		if (setegid(saved_id_info->gid) == -1) {
			perror("setegid");
			exit(EXIT_FAILURE);
		}
	}

	/* If we were root, set groups */
	if (saved_id_info->uid == 0) {
		if (setgroups(saved_id_info->num_groups, saved_id_info->groups) == -1) {
			perror("setgroups");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}


