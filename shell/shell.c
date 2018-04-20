//
//  shell.c
//  shell
//
//  Created by Adam on 4/11/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "shell.h"

#include "constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "job_parser.h"
#include "executor.h"
#include "error.h"

#define BUF_SIZE 514
#define SHELL "520sh> "

struct shell {
	size_t runs;
	shell_mode mode;
	char * filename;
};

shell_handle create_shell_with_filename(const char * filename)
{
	shell_handle new = (shell_handle)malloc(sizeof(*new));
	new->mode = file_m;
	new->filename = strdup(filename);
	new->runs = 0;
	return new;
}

shell_handle create_shell(void)
{
	shell_handle new = (shell_handle)malloc(sizeof(*new));
	new->mode = normal_m;
	new->filename = NULL;
	new->runs = 0;
	return new;
}

bool all_ending_or_space(char * string) {
	size_t length = strlen(string);
	for (int i = 0; i < length; i++)
	{
		char c = string[i];
		//there needs to be something other than these terms
		if (!isspace(c) && c != NULL_TERM && c != SEMICOLON && c != PLUS && c != PIPE && c != REDIRECT) {
			return false;
		}
	}
	return true;
}

void start_shell(shell_handle shell)
{
	if (shell->mode == file_m) {
		
		FILE * fp = fopen(shell->filename, "r");
		if (fp == NULL) {
			print_error_and_exit();
		}
		
		char buffer[BUF_SIZE];
		
		while (fgets(buffer, BUF_SIZE, fp) != NULL) {
			if (buffer[strlen(buffer) - 1] != '\n') { //command was too long
				print_error_and_exit();
			}
			
			job_parser_handle job_parser = create_job_parser(buffer);
			jobs_handle jobs = run_job_parser(job_parser);
			if (jobs == NULL) { //jobs can't have more than two in one
				print_error_and_exit(); //input was incorrectly formatted
			}
			executor_handle executor = create_executor(jobs);
			run(executor);
			destroy_executor(executor);
			destroy_jobs(jobs);
			destroy_job_parser(job_parser);
			(shell->runs)++;
		}
		
		fclose(fp);
	}
	else {
		//shell
		while (true) {
			fprintf(stdout, "%s", SHELL);
			char buffer[BUF_SIZE];
			fgets(buffer, BUF_SIZE, stdin);
			if (buffer[strlen(buffer) - 1] != '\n') { //command was too long
				print_error_and_exit();
			}
			if (all_ending_or_space(buffer)) {
				continue;
			}
			job_parser_handle job_parser = create_job_parser(buffer);
			jobs_handle jobs = run_job_parser(job_parser);
			if (jobs == NULL) { //jobs can't have more than two in one
				print_error_and_exit(); //input was incorrectly formatted
			}
			executor_handle executor = create_executor(jobs);
			run(executor);
			destroy_executor(executor);
			destroy_jobs(jobs);
			destroy_job_parser(job_parser);
			(shell->runs)++;
		}
	}
}

void destroy_shell(shell_handle shell) {
	if (shell->mode == file_m && shell->filename != NULL) {
		free(shell->filename);
	}
	free(shell);
}
