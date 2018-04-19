//
//  job_parser.c
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "job_parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "constants.h"
#include "parser_helper.h"

#define BUF_SIZE 514

job_parser_handle create_job_parser(char * command_string)
{
	job_parser_handle new = (job_parser_handle)malloc(sizeof(*new));
	new->command_string = strdup(command_string);
	return new;
}

void process_job_segment(char * command_line, size_t * index, size_t * num_commands, char *** command_strings, size_t ** arg_counts, char **** args)
{
	//enter job segment
	while (!is_job_segment_end(command_line[*index])) { //will be here after job end or pipe
		while (is_ignored_space(command_line[*index])) {
			(*index)++;
		}
		//get till first word end (command)
		char command_buffer[BUF_SIZE];
		size_t command_index = 0;
		while (command_line[*index] != NULL_TERM && !is_job_end(command_line[*index]) && !is_ignored_space(command_line[*index])) {
			command_buffer[command_index++] = command_line[(*index)++];
		}
		command_buffer[command_index] = '\0';
		(*num_commands)++; //increment commands
		*command_strings = (char **)realloc(*command_strings, sizeof(**command_strings) * (*num_commands)); //increase array size
		*args = (char ***)realloc(*args, sizeof(char **) * (*num_commands)); //increase arg array of array size
		(*command_strings)[*num_commands - 1] = strdup(command_buffer);
		
		size_t arg_count = 0;
		(*arg_counts) = (size_t *)realloc(*arg_counts, sizeof(size_t) * (*num_commands));
		(*arg_counts)[*num_commands - 1] = arg_count;
		(*args)[*num_commands - 1] = (char **)malloc(sizeof(char *));
		if (is_job_end(command_line[*index])) { //no arguments
			return;
		}
		else { //is_ignored_space is true, consume and see if it's a job segment end
			while (is_ignored_space(command_line[*index])) {
				(*index)++;
			}
			if (!is_job_end(command_line[*index]) && !is_pipe_or_redirect(command_line[*index])) { //arguments
				while (!is_job_end(command_line[*index]) && !is_pipe_or_redirect(command_line[*index])) {
					char arg_buffer[BUF_SIZE];
					size_t arg_index = 0;
					while (command_line[*index] != NULL_TERM && !is_job_end(command_line[*index]) && !is_ignored_space(command_line[*index])) {
						arg_buffer[arg_index++] = command_line[(*index)++];
					}
					arg_buffer[arg_index] = '\0';
					arg_count++;
					(*args)[*num_commands - 1] = (char **)realloc((*args)[*num_commands - 1], sizeof(char *) * arg_count);
					(*args)[*num_commands - 1][arg_count - 1] = strdup(arg_buffer);
					
					while (is_ignored_space(command_line[*index])) {
						(*index)++;
					}
				}
				(*arg_counts)[*num_commands - 1] = arg_count;
			}
		}
	}
}

void process_job(char * command_line, size_t * index, size_t * num_commands, job_type * job_type, char *** command_strings, size_t ** arg_counts, char **** args)
{
	//enter job
	while (command_line[*index] != NULL_TERM && !is_job_end(command_line[*index])) { //will be here at beginning, after ;, after +
		while (is_ignored_space(command_line[*index])) {
			(*index)++;
		}
		//beginning of job (or job segment)
		process_job_segment(command_line, index, num_commands, command_strings, arg_counts, args);
		if (command_line[*index] == PIPE) {
			*job_type = piped_t;
		}
		else if (command_line[*index] == REDIRECT) {
			*job_type = redirection_t;
		}
		if (is_pipe_or_redirect(command_line[*index])) {
			(*index)++;
		}
		//trim the rest of whitespace in case the line end is in the distance
		while (is_ignored_space(command_line[*index])) {
			(*index)++;
		}
	}
}

jobs_handle run_job_parser(job_parser_handle job_parser)
{
	char * command_line = strdup(job_parser->command_string);
	size_t index = 0;
	
	//set up data for jobs_handle
	jobs_mode jobs_mode = serial_m;
	size_t num_jobs = 0;
	job_handle * jobs = (job_handle *)malloc(sizeof(*jobs));
	bool parallel = false;
	bool serial = false;
	
	while (index < strlen(command_line) && !is_line_end(command_line[index])) { //will come here each time a job is added
		
		//set up data for the job_handle
		size_t num_commands = 0; //for the individual command
		job_type job_type = normal_t; //redirection, pipe or normal
		char ** command_strings = (char **)malloc(sizeof(char *));
		command_type * command_types = (command_type *)malloc(sizeof(command_type));
		size_t * arg_counts = (size_t *)malloc(sizeof(size_t));
		char *** args = (char ***)malloc(sizeof(char **));
		
		//enter job
		process_job(command_line, &index, &num_commands, &job_type, &command_strings, &arg_counts, &args);
		
		//job is ready to be added
		num_jobs++;
		jobs = (job_handle *)realloc(jobs, sizeof(job_handle) * num_jobs);
		jobs[num_jobs - 1] = create_job(num_commands, command_strings, arg_counts, args, job_type);
		
		//free up stuff
		for (int i = 0; i < num_commands; i++)
		{
			free(command_strings[i]);
		}
		free(command_strings);
		free(command_types);
		for (int i = 0; i < num_commands; i++)
		{
			for (int j = 0; j < arg_counts[i]; j++)
			{
				free(args[i][j]);
			}
			free(args[i]);
		}
		free(arg_counts);
		free(args);
		
		//process end of the line characters
		if (command_line[index] == SEMICOLON) {
			serial = true;
		}
		else if (command_line[index] == PLUS) {
			parallel = true;
		}
		index++; //if it's end of line, will end normally
		//otherwise cycle through junk
		while (is_ignored_space(command_line[index]) || is_job_end(command_line[index])) {
			if (command_line[index] == SEMICOLON) {
				serial = true;
			}
			else if (command_line[index] == PLUS) {
				parallel = true;
			}
			(index)++;
		}
	}
	
	free(command_line);
	
	if (parallel && serial) { //badly formatted input (;'s and +'s)
		for (int i = 0; i < num_jobs; i++)
		{
			destroy_job(jobs[i]);
		}
		free(jobs);
		return NULL;
	}
	
	if (parallel) {
		jobs_mode = parallel_m;
	}
	else if (serial) {
		jobs_mode = serial_m;
	}
	
	jobs_handle result = create_jobs(jobs_mode, jobs, num_jobs);
	
	for (int i = 0; i < num_jobs; i++)
	{
		destroy_job(jobs[i]);
	}
	free(jobs);
	
	return result;
}

void destroy_job_parser(job_parser_handle job_parser)
{
	free(job_parser->command_string);
	free(job_parser);
}
