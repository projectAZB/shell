//
//  job.c
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "job.h"
#include "constants.h"

#include <string.h>
#include <assert.h>
#include <stdbool.h>

command_type command_type_from_string(const char * command_string)
{
	if (strcmp(QUIT, command_string) == 0) {
		return built_in_t;
	}
	else if (strcmp(PWD, command_string) == 0) {
		return built_in_t;
	}
	else if (strcmp(CD, command_string) == 0) {
		return built_in_t;
	}
	else {
		return external_t;
	}
}

job_handle create_job(size_t num_commands, char ** command_strings, size_t * arg_counts, char *** args, job_type job_type)
{
	job_handle new = (job_handle)malloc(sizeof(*new));
	new->num_commands = num_commands;
	new->command_strings = (char **)malloc(sizeof(char *) * num_commands);
	new->arg_counts = (size_t *)malloc(sizeof(size_t) * num_commands);
	new->args = (char ***)malloc(sizeof(char **) * num_commands);
	new->command_types = (command_type *)malloc(sizeof(command_type) * num_commands);
	new->job_type = job_type;
	for (int i = 0; i < new->num_commands; i++)
	{
		new->command_strings[i] = strdup(command_strings[i]);
		new->arg_counts[i] = arg_counts[i];
		new->args[i] = arg_counts[i] == 0 ? (char **)malloc(sizeof(char *)) : (char **)malloc(sizeof(char *) * arg_counts[i]);
		for (int j = 0; j < arg_counts[i]; j++)
		{
			new->args[i][j] = strdup(args[i][j]);
		}
		new->command_types[i] = command_type_from_string(command_strings[i]);
	}
	return new;
}

job_handle copy_job(const job_handle job)
{
	return create_job(job->num_commands, job->command_strings, job->arg_counts, job->args, job->job_type);
}

void destroy_job(job_handle job)
{
	for (int i = 0; i < job->num_commands; i++)
	{
		free(job->command_strings[i]);
		for (int j = 0; j < job->arg_counts[i]; j++)
		{
			free(job->args[i][j]);
		}
		free(job->args[i]);
	}
	free(job->command_types);
	free(job->args);
	free(job->arg_counts);
	free(job->command_strings);
	free(job);
}

char ** argv_for_job(job_handle job, int command_index)
{
	if (command_index >= job->num_commands) {
		assert(false);
	}
	size_t arg_count = job->arg_counts[command_index];
	char ** argv = (char **)malloc(sizeof(char *) * (arg_count + 1)); //arg strings and one for command_string
	int index = 0;
	argv[index++] = strdup(job->command_strings[command_index]);
	for (int i = 0; i < arg_count; i++) {
		argv[index++] = strdup(job->args[command_index][i]);
	}
	return argv;
}

size_t argc_for_job(job_handle job, int command_index)
{
	return (job->arg_counts[command_index] + 1);
}

void destroy_argv(char ** argv, int argc)
{
	for (int i = 0; i < argc; i++)
	{
		free(argv[i]);
	}
	free(argv);
}



