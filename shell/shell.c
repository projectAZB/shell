//
//  shell.c
//  shell
//
//  Created by Adam on 4/11/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "job_parser.h"

#define BUF_SIZE 514
#define SHELL "520sh> "

void print_error();

struct shell {
	size_t runs;
};

shell_handle create_shell(void)
{
	shell_handle new = (shell_handle)malloc(sizeof(*new));
	new->runs = 0;
	return new;
}

void start_shell(shell_handle shell)
{
	//shell
	while (true) {
		fprintf(stdout, "%s", SHELL);
		char buffer[BUF_SIZE];
		fgets(buffer, BUF_SIZE, stdin);
		if (buffer[strlen(buffer) - 1] != '\n') { //command was too long
			print_error();
			continue; // loop around again
		}
		job_parser_handle job_parser = create_job_parser(buffer);
		commands_handle * commands = run_job_parser(job_parser);
		if (commands == NULL) {
			print_error(); //input was incorrectly formatted
			destroy_job_parser(job_parser);
		}
		destroy_job_parser(job_parser);
		printf("\n");
		(shell->runs)++;
	}
}

void destroy_shell(shell_handle shell) {
	free(shell);
}

void print_error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDOUT_FILENO, error_message, 30);
}
