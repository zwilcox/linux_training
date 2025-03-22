#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LOGIN_STR	("login ")
#define PASSWORD_STR	("password ")
#define RESET_STR	("reset")
#define ADMIN_STR	("admin")

typedef struct auth_info {
	char username[32];
	int is_admin;
} auth_info_t;

int main(int argc, char* argv[]) {
	char admin_pass[] = "dontuseme";
	char line[128];
	auth_info_t* ai;
	char* password = NULL;
	char* arg;

	while(1) {
		if (fgets(line, sizeof(line), stdin) == NULL) {
			break;
		}

		if (strncmp(line, LOGIN_STR, sizeof(LOGIN_STR)-1) == 0) {
			arg = line + sizeof(LOGIN_STR)-1;
			ai = (auth_info_t*)malloc(sizeof(auth_info_t));
			if (ai == NULL) {
				fprintf(stderr, "Failed to malloc\n");
				return EXIT_FAILURE;
			}
			strncpy(ai->username, arg, 31);
			ai->is_admin = 0;
		}
		else if (strncmp(line, PASSWORD_STR, sizeof(PASSWORD_STR)-1) == 0) {
			arg = line + sizeof(PASSWORD_STR)-1;
			password = strdup(arg);
			password[strlen(password)-1] = '\0';
			if (strcmp(password, admin_pass) == 0) {
				printf("passwords match\n");
				ai->is_admin = 1;
			}
		}
		else if (strncmp(line, RESET_STR, sizeof(RESET_STR)-1) == 0) {
			ai->is_admin = NULL;
			free(ai);
		}
		else if (strncmp(line, ADMIN_STR, sizeof(ADMIN_STR)-1) == 0) {
			if (ai->is_admin) {
				printf("You have admin powers\n");
				return EXIT_SUCCESS;
			}
			else {
				printf("You must enter an admin password first\n");
			}
		}
		else {
			printf("Unrecognized command!\n");
		}
	}
	return EXIT_SUCCESS;
}

