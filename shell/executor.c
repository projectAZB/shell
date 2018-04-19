//
//  executor.c
//  shell
//
//  Created by Adam on 4/17/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "executor.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "built_in.h"

struct executor {
	jobs_handle jobs;
};

executor_handle create_executor(jobs_handle jobs)
{
	executor_handle new = (executor_handle)malloc(sizeof(*new));
	new->jobs = jobs;
	return new;
}

void run_parallel_job(executor_handle executor)
{
	
}

void run_serial_job(executor_handle executor)
{
	jobs_handle jobs = executor->jobs;
	size_t num_jobs = jobs->num_jobs;
	for (int i = 0; i < num_jobs; i++)
	{
		job_handle job = jobs->jobs[i];
		job_type job_type = job->job_type;
		char ** argv = argv_for_job(job, 0);
		size_t argc = argc_for_job(job, 0);
		
		if (job_type == normal_t) {
			if (job->command_types[0] == built_in_t) { //no need to fork a process for the built-in
				run_built_in_command_with_args(argv, argc);
			}
			else {
				pid_t pid = fork();
				if (pid == -1) { //fork failed
					exit(5);
				}
				if (pid == 0) { //child process
					execvp(argv[0], argv);
				}
				else { //parent process, pid non-zero
					waitpid(pid, NULL, 0);
					char buffer[] = "done\n";
					write(STDOUT_FILENO, buffer, strlen(buffer));
				}
				char buffer[] = "external\n";
				write(STDOUT_FILENO, buffer, strlen(buffer));
			}
		}
		else if (job_type == redirection_t) {
			
		}
		else if (job_type == piped_t) {
			
		}
		else {
			assert(false); // should only be one of the three types
		}
		
		destroy_argv(argv, argc);
	}
}

void run(executor_handle executor)
{
	if (executor->jobs->jobs_mode == parallel_m) {
		run_parallel_job(executor);
	}
	else {
		run_serial_job(executor);
	}
}

void destroy_executor(executor_handle executor)
{
	free(executor);
}
