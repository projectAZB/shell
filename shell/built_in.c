//
//  built_in.c
//  shell
//
//  Created by Adam on 4/18/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "built_in.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "constants.h"
#include "error.h"

void run_quit() {
	exit(EXIT_SUCCESS);
}

void run_pwd(char ** argv, size_t argc) {
	if (argc > 1) {
		print_error();
		return;
	}
	size_t size = pathconf(".", _PC_PATH_MAX);
	char * buffer = (char *)malloc(sizeof(char) * size);
	getcwd(buffer, size);
	write(STDOUT_FILENO, buffer, strlen(buffer));
	char newline[] = "\n";
	write(STDOUT_FILENO, newline, strlen(newline));
	free(buffer);
}

void run_cd(char ** argv, size_t argc) {
	if (argc == 1) { //just cd
		const char * home = "HOME";
		char * value;
		value = getenv(home);
		chdir(value);
	}
	else if (argc == 2) {
		int res = chdir(argv[1]);
		if (res < 0) {
			print_error();
			return;
		}
	}
	else { //shouldn't be more than 2
		print_error();
		return;
	}
}

void run_built_in_command_with_args(char ** argv, size_t argc)
{
	char * command_string = argv[0];
	if (strcmp(QUIT, command_string) == 0) {
		run_quit();
	}
	else if (strcmp(PWD, command_string) == 0) {
		run_pwd(argv, argc);
		return;
	}
	else if (strcmp(CD, command_string) == 0) {
		run_cd(argv, argc);
		return;
	}
	else {
		assert(false);
	}
}
