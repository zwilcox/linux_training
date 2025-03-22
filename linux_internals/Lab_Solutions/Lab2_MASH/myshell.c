#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <errno.h>
#include <signal.h>

char** create_argv(char* line, int* argc);
void argv_free(char** args);
void sig_handler(int signum);
void handle_single_command(char* line_buffer, int is_bg);
void handle_piped_command(char* line_one, char* line_two, int is_bg);

pid_t fg_process;
pid_t bg_process;

int main() {
	int status;
	int is_bg;
	bg_process = -1;
	fg_process = -1;
    struct sigaction sa = {0};

    sa.sa_handler = sig_handler;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
	}

    size_t chars_read;
    char* line_buffer;
    line_buffer = NULL;
    while (1) {
		printf(">> ");
        chars_read = getline(&line_buffer, &chars_read, stdin);
        if (chars_read == -1) {
            if (feof(stdin)) {
                break;
            }
            perror("getline");
            continue;
        }
		if (line_buffer[chars_read - 1] == '\n') {
			line_buffer[chars_read - 1] = '\0';
			chars_read -= 1;
		}
		
		printf("line_buffer is %s\n", line_buffer);
		
		if (strncmp(line_buffer, "exit", sizeof("exit")-1) == 0 ||
			strncmp(line_buffer, "quit", sizeof("quit")-1) == 0) {
			break;
		}
		else if (strncmp(line_buffer, "fg", sizeof("fg")-1) == 0) {
			fg_process = bg_process;
			bg_process = -1;
	        if (waitpid(fg_process, &status, 0) == -1) {
	            perror("waitpid");
	        }
		}
		else {
			is_bg = 0;
			if (line_buffer[chars_read - 1] == '&') {
				is_bg = 1;
				line_buffer[chars_read - 1] = '\0';
				chars_read -= 1;
			}

			char* pipe_chr_pos;
			pipe_chr_pos = strchr(line_buffer, '|');
			if (pipe_chr_pos != NULL) {
				*pipe_chr_pos = '\0';
				printf("first command is %s and second command is %s\n", line_buffer, pipe_chr_pos + 1);
				handle_piped_command(line_buffer, pipe_chr_pos + 1, is_bg);
			}
			else {
				handle_single_command(line_buffer, is_bg);
			}
		}
    }

    free(line_buffer);
	return EXIT_SUCCESS;
}

void handle_single_command(char* line_buffer, int is_bg) {
	int status;
	char** args;
	int num_args;
	pid_t pid;
    struct sigaction sa = {0};
	args = create_argv(line_buffer, &num_args);
	if (args == NULL) {
		printf("Command could not be interpreted\n");
		return;
	}
	pid = fork();
	if (pid == -1) {
		argv_free(args);
		perror("fork");
		return;
	}
	else if (pid == 0) {
		/* reset signal handler to default */
 		sa.sa_handler = SIG_DFL;
	    if (sigaction(SIGINT, &sa, NULL) == -1) {
	        perror("sigaction");
	    }
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		exit(EXIT_FAILURE);
	}
	else {
		argv_free(args);
		if (is_bg == 0) {
			fg_process = pid;
	        if (waitpid(fg_process, &status, 0) == -1) {
	            perror("waitpid");
	        }
			fg_process = -1;
		}
		else {
			bg_process = pid;
		}
	}
	return;
}

void handle_piped_command(char* line_one, char* line_two, int is_bg) {
	int pipe_fd[2];
	char** args;
	int num_args;
	int status;
	pid_t pid_one;
	pid_t pid_two;
    struct sigaction sa = {0};
	
	if (pipe(pipe_fd) == -1) {
		perror("pipe");
		return;
	}

	pid_one = fork();
	if (pid_one == -1) {
		perror("fork");
		return;
	}

	if (pid_one == 0) {
		args = create_argv(line_one, &num_args);
		if (args == NULL) {
			printf("First command could not be interpreted\n");
			exit(EXIT_FAILURE);
		}
		sa.sa_handler = SIG_DFL;
		if (sigaction(SIGINT, &sa, NULL) == -1) {
			perror("sigaction");
		}

		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		exit(EXIT_FAILURE);
	}
	else { // I'm the parent
		pid_two = fork();
		if (pid_two == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid_two == 0) {
			args = create_argv(line_two, &num_args);
			if (args == NULL) {
				printf("Second command could not be interpreted\n");
				exit(EXIT_FAILURE);
			}
			sa.sa_handler = SIG_DFL;
			if (sigaction(SIGINT, &sa, NULL) == -1) {
				perror("sigaction");
			}

			close(pipe_fd[1]);
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
			if (execvp(args[0], args) == -1) {
				perror("execvp");
			}
			exit(EXIT_FAILURE);
		}
		else { /* real parent */
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			if (is_bg == 0) {
				fg_process = pid_two;
				if (waitpid(fg_process, &status, 0) == -1) {
					perror("waitpid");
				}
				fg_process = -1;
			}
			else {
				bg_process = pid_two;
			}
		}
	}
	return;
}

void handle_n_pipe_commands(char** lines, unsigned int n, int is_bg) {
	unsigned int i;
	char** args;
	int num_args;
	pid_t pid;
    struct sigaction sa = {0};
	int pipe_fd[2];
	int next_pipe_fd[2];
	int status;
	for (i = 0; i < n; i++) {
		args = create_argv(lines[i], &num_args);
		if (args == NULL) {
			printf("Command %u could not be interpreted\n", i);
			return;
		}
		if (pipe(next_pipe_fd) == -1) {
			perror("pipe");
			return;
		}
		pid = fork();
		if (pid == -1) {
			perror("fork");
			return;
		}
		if (pid == 0) {
	 		sa.sa_handler = SIG_DFL;
		    if (sigaction(SIGINT, &sa, NULL) == -1) {
		        perror("sigaction");
		    }
			if (i != 0) {
				close(pipe_fd[1]);
				dup2(pipe_fd[0], STDIN_FILENO);
				close(pipe_fd[0]);
			}

			/* set output to next pipe */
			if (i != (n -1)) {
				close(next_pipe_fd[0]);
				dup2(next_pipe_fd[1], STDOUT_FILENO);
				close(next_pipe_fd[1]);	
			}

			if (execvp(args[0], args) == -1) {
				perror("execvp");
			}
			exit(EXIT_FAILURE);
		}

		if (i != 0) {
			close(pipe_fd[0]);
			close(pipe_fd[1]);
		}
		pipe_fd[1] = next_pipe_fd[1];
		pipe_fd[0] = next_pipe_fd[0];

	}

	/* shell process */
	argv_free(args);
	if (is_bg == 0) {
		fg_process = pid;
        if (waitpid(fg_process, &status, 0) == -1) {
            perror("waitpid");
        }
		fg_process = -1;
	}
	else {
		bg_process = pid;
	}
		
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	return;
}
char** create_argv(char* line, int* argc) {
	int i;
	char** argv;
	char* arg;
	wordexp_t p;
	if (wordexp(line, &p, 0) != 0) {
		return NULL;
	}
	argv = malloc(sizeof(char*) * (p.we_wordc + 1));
	if (argv == NULL) {
		printf("Afd\n");
		goto fail;
	}

	for (i = 0; i < p.we_wordc; i++) {
		arg = strdup(p.we_wordv[i]);
		if (arg == NULL) {
			goto fail;
		}
		argv[i] = arg;
		printf("argv[%d] = %s\n", i, arg);
	}
	argv[i] = NULL;

	wordfree(&p);
	*argc = p.we_wordc;
	return argv;

fail:
	wordfree(&p);
	return NULL;
}

void argv_free(char** args) {
	int i;
	i = 0;
	while (args[i] != NULL) {
		free(args[i]);
		i++;
	}
	free(args);
	return;
}

void sig_handler(int signum) {
	int saved_errno;
    if (signum == SIGINT) {
		if (fg_process != -1) {
			kill(fg_process, SIGINT);
		}
	}
	else if (signum == SIGCHLD) {
		saved_errno = errno;
		while (waitpid(-1, NULL, WNOHANG) > 0) {}
		errno = saved_errno;
	}
    return;
}
