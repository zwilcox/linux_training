#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <errno.h>

char** create_argv(char* line, int* argc);
void argv_free(char** args);
void handle_single_command(char* line_buffer);
void handle_piped_command(char* line_one, char* line_two);


int main() {
    size_t chars_read;
    char* line_buffer = NULL;
	char* pipe_position = NULL;

    while (1) {
		printf(">> ");
        chars_read = getline(&line_buffer, &chars_read, stdin);
        if (chars_read == -1) {
			/* end of file check */
            if (feof(stdin)) {
                break;
            }
			/* error check */
            perror("getline");
            continue;
        }
		/* getline will capture newline - replace with null terminator so string functions work correctly */ 
		if (line_buffer[chars_read - 1] == '\n') {
			line_buffer[chars_read - 1] = '\0';
			chars_read -= 1;
		}
		
		printf("line_buffer is %s\n", line_buffer);
		
		/* LAB task - check keywords */
		if (strncmp(line_buffer, "exit", sizeof("exit")-1) == 0 ||
			strncmp(line_buffer, "quit", sizeof("quit")-1) == 0) {
			break;
		}
		else {
			/* search for pipe character in line */
			//char* pipe_position = NULL;
			pipe_position = strchr(line_buffer, '|');
			if (pipe_position != NULL) {
				/* pipe exists, so split the lines into two strings (line_buffer and pipe_position + 1) */
				*pipe_position = '\0';
				printf("first command is %s and second command is %s\n", line_buffer, pipe_position + 1);
				handle_piped_command(line_buffer, pipe_position + 1);
			}
			else {
				/* no pipe character */
				handle_single_command(line_buffer);
			}
		}
    }

	/* free buffers */
    free(line_buffer);
	line_buffer = NULL;
	if (pipe_position) {
		free(pipe_position);
		pipe_position = NULL;
	}
	return EXIT_SUCCESS;
}

void handle_single_command(char* line_buffer) {
	char** args;
	int num_args;

	args = create_argv(line_buffer, &num_args);
	if (args == NULL) {
		printf("Command could not be interpreted\n");
		return;
	}

	/* create child for exec */
	pid_t pid;
	pid = fork();
	if (pid == -1) {
		argv_free(args);
		perror("fork");
		return;
	}
	/* pid == 0 is the child */
	else if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		exit(EXIT_FAILURE);
	}
	/* parent waits for child to finish */
	else {
		argv_free(args);
		int status;
	    if (waitpid(pid, &status, 0) == -1) {
	        perror("waitpid");
	    }
	}

	return;
}

void handle_piped_command(char* line_one, char* line_two) {
	char** args;
	int num_args;
	
	/* create pipe for children to use */
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1) {
		perror("pipe");
		return;
	}

	/* create first child for exec */
	pid_t pid_one;
	pid_one = fork();
	if (pid_one == -1) {
		perror("fork");
		return;
	}
	/* pid == 0 is the child */
	if (pid_one == 0) {
		args = create_argv(line_one, &num_args);
		if (args == NULL) {
			printf("First command could not be interpreted\n");
			exit(EXIT_FAILURE);
		}
		/* use dup2 to make stdout the write end of child one's pipe */
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]); // close write end we dup2'ed
		close(pipe_fd[0]); // close read end
		if (execvp(args[0], args) == -1) {
			perror("execvp");
		}
		exit(EXIT_FAILURE);
	}
	else {
		/* create second child for exec */
		pid_t pid_two;
		pid_two = fork();
		if (pid_two == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		/* pid == 0 is the child */
		if (pid_two == 0) {
			args = create_argv(line_two, &num_args);
			if (args == NULL) {
				printf("Second command could not be interpreted\n");
				exit(EXIT_FAILURE);
			}
			/* use dup2 to make stdin the read end of child two's pipe */
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]); // close read end we dup2'ed
			close(pipe_fd[1]); // close write end
			if (execvp(args[0], args) == -1) {
				perror("execvp");
			}
			exit(EXIT_FAILURE);
		}
		/* parent */
		else {
			/* parent isn't using the pipe so close both ends */
			close(pipe_fd[0]); 
			close(pipe_fd[1]);
			int status;
			if (waitpid(pid_two, &status, 0) == -1) {
					perror("waitpid");
			}
		}
	}

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
