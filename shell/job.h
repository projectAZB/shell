//
//  job.h
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef job_h
#define job_h

#include <stdlib.h>

#include "constants.h"

enum command_type {
	built_in_t,
	external_t
};

typedef enum command_type command_type;

struct job {
	job_type job_type; //redirection, pipe, or normal
	
	size_t num_commands;
	
	char ** command_strings; //num_commands determines this size
	command_type * command_types; //num_commands determines this size
	
	size_t * arg_counts; //num_commands determines this size
	char *** args;
};

typedef struct job job;

typedef job * job_handle;

job_handle create_job(size_t num_jobs, char ** command_strings, size_t * arg_counts, char *** args, job_type job_type);
job_handle copy_job(const job_handle job);
void destroy_job(job_handle job);

#endif
