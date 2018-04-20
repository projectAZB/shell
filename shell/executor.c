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
#include <fcntl.h>

#include "built_in.h"
#include "error.h"

#define READ_END 0
#define WRITE_END 1

struct executor {
	jobs_handle jobs;
};

executor_handle create_executor(jobs_handle jobs)
{
	executor_handle new = (executor_handle)malloc(sizeof(*new));
	new->jobs = jobs;
	return new;
}

void run_normal_job(char ** argv, pid_t * pid, bool serial)
{
	*pid = fork();
	if (*pid == -1) { //fork failed
		exit(5);
	}
	else if (*pid == 0) { //child process
		int res = execvp(argv[0], argv);
		//this will never execute if execvp works
		if (res < 0) {
			print_error_and_exit();
		}
	}
	else { //parent process, pid non-zero
		if (serial) { //if it's serial wait for the job to finish
			waitpid(*pid, NULL, 0);
		}
	}
}

void run_redirection_job(char ** argv, char ** argv2, pid_t * pid, bool serial)
{
	*pid = fork();
	if (*pid == -1) { //fork failed
		exit(5);
	}
	else if (*pid == 0) { //child process
		int output_fd = open(argv2[0], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		int dup_res = dup2(output_fd, STDOUT_FILENO);
		if (dup_res == -1) {
			print_error_and_exit();
		}
		
		int exec_res = execvp(argv[0], argv);
		//this will never execute if execvp works
		if (exec_res < 0) {
			print_error_and_exit();
		}
	}
	else { //parent process, pid non-zero
		if (serial) { //if it's serial wait for the job to finish
			waitpid(*pid, NULL, 0);
		}
	}
}

void run_pipe_write(int * pipe_fds, char ** argv, pid_t * pid)
{
	*pid = fork();
	if (*pid == -1) { //fork failed
		exit(5);
	}
	else if (*pid == 0) { //child process, handles the write end "write end | read end"
		close(pipe_fds[READ_END]);
		int dup_res = dup2(pipe_fds[WRITE_END], STDOUT_FILENO); // both of these macros are 1
		
		if (dup_res == -1) {
			print_error_and_exit();
		}
		
		int exec_res = execvp(argv[0], argv);
		//this will never execute if execvp works
		if (exec_res < 0) {
			print_error_and_exit();
		}
	}
	else { //parent
		return;
	}
}

void run_pipe_read(int * pipe_fds, char ** argv, pid_t * pid)
{
	*pid = fork();
	if (*pid == -1) { //fork failed
		exit(7);
	}
	else if (*pid == 0) {
		int dup_res = dup2(pipe_fds[READ_END], STDIN_FILENO); //both of these macros are 0
		close(pipe_fds[WRITE_END]);
		
		if (dup_res == -1) {
			print_error_and_exit();
		}
		
		int exec_res = execvp(argv[0], argv);
		//this will never execute if execvp works
		if (exec_res < 0) {
			print_error_and_exit();
		}
	}
	else { //back in the parent process now
		return;
	}
}

void run_serial_job(executor_handle executor)
{
	jobs_handle jobs = executor->jobs;
	size_t num_jobs = jobs->num_jobs;
	for (int i = 0; i < num_jobs; i++)
	{
		job_handle job = jobs->jobs[i];
		
		if (job->num_commands > 2) { //can't have more than 2 commands per job
			print_error_and_exit();
		}
		
		job_type job_type = job->job_type;
		char ** argv = argv_for_job(job, 0);
		size_t argc = argc_for_job(job, 0);
		
		if (job_type == normal_t) {
			if (job->command_types[0] == built_in_t) { //no need to fork a process for the built-in
				run_built_in_command_with_args(argv, argc);
			}
			else {
				pid_t pid;
				run_normal_job(argv, &pid, true);
			}
		}
		else if (job_type == redirection_t) {
			//can't be built in and redirected
			if (job->command_types[0] == built_in_t || job->command_types[1] == built_in_t) {
				print_error_and_exit();
			}
			
			pid_t pid;
			
			char ** argv2 = argv_for_job(job, 1); //get the second job segment part (file to reroute to)
			size_t argc2 = argc_for_job(job, 1);
			
			run_redirection_job(argv, argv2, &pid, true);
			
			destroy_argv(argv2, argc2);
		}
		else if (job_type == piped_t) {
			//can't be built in and piped
			if (job->command_types[0] == built_in_t || job->command_types[1] == built_in_t) {
				print_error_and_exit();
			}
			
			int pipe_fds[2]; //[0] is for using read end, [1] for using write end
			int pipe_res = pipe(pipe_fds);
			
			if (pipe_res == -1) {
				print_error_and_exit();
			}
			
			if (pipe_fds[0] == -1 || pipe_fds[1] == -1) {
				print_error_and_exit();
			}
			
			char ** argv2 = argv_for_job(job, 1); //get the second job segment part (file to reroute to)
			size_t argc2 = argc_for_job(job, 1);
			
			pid_t pids[2];
			
			run_pipe_write(pipe_fds, argv, &pids[1]);
			run_pipe_read(pipe_fds, argv2, &pids[0]);
			
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			
			for (int x = 0; x < 2; x++) //this is the serial part
			{
				waitpid(pids[x], NULL, 0);
			}
			
			destroy_argv(argv2, argc2);
		}
		else {
			assert(false); // should only be one of the three types
		}
		destroy_argv(argv, argc);
	}
}

void run_parallel_job(executor_handle executor)
{
	jobs_handle jobs = executor->jobs;
	size_t num_jobs = jobs->num_jobs;
	pid_t * pids_to_wait = (pid_t *)malloc(sizeof(pid_t));
	size_t num_pids = 0;
	for (int i = 0; i < num_jobs; i++)
	{
		job_handle job = jobs->jobs[i];
		
		if (job->num_commands > 2) { //can't have more than 2 commands per job
			print_error_and_exit();
		}
		
		job_type job_type = job->job_type;
		char ** argv = argv_for_job(job, 0);
		size_t argc = argc_for_job(job, 0);
		
		if (job_type == normal_t) {
			if (job->command_types[0] == built_in_t) { //no need to fork a process for the built-in
				run_built_in_command_with_args(argv, argc);
			}
			else {
				pid_t pid;
				run_normal_job(argv, &pid, false);
				//add pid to wait array
				num_pids++;
				pids_to_wait = (pid_t *)realloc(pids_to_wait, sizeof(pid_t) * num_pids);
				pids_to_wait[num_pids - 1] = pid;
			}
		}
		else if (job_type == redirection_t) {
			//can't be built in and redirected
			if (job->command_types[0] == built_in_t || job->command_types[1] == built_in_t) {
				print_error_and_exit();
			}
			
			pid_t pid;
			
			char ** argv2 = argv_for_job(job, 1); //get the second job segment part (file to reroute to)
			size_t argc2 = argc_for_job(job, 1);
			
			run_redirection_job(argv, argv2, &pid, false);
			//add pid to wait array
			num_pids++;
			pids_to_wait = (pid_t *)realloc(pids_to_wait, sizeof(pid_t) * num_pids);
			pids_to_wait[num_pids - 1] = pid;
			
			destroy_argv(argv2, argc2);
		}
		else if (job_type == piped_t) {
			//can't be built in and piped
			if (job->command_types[0] == built_in_t || job->command_types[1] == built_in_t) {
				print_error_and_exit();
			}
			
			int pipe_fds[2]; //[0] is for using read end, [1] for using write end
			int pipe_res = pipe(pipe_fds);
			
			if (pipe_res == -1) {
				print_error_and_exit();
			}
			
			if (pipe_fds[0] == -1 || pipe_fds[1] == -1) {
				print_error_and_exit();
			}
			
			char ** argv2 = argv_for_job(job, 1); //get the second job segment part (file to reroute to)
			size_t argc2 = argc_for_job(job, 1);
			
			pid_t pids[2];
			
			run_pipe_write(pipe_fds, argv, &pids[1]);
			run_pipe_read(pipe_fds, argv2, &pids[0]);
			
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			
			for (int x = 0; x < 2; x++) //this is the parallel part, add to wait after all jobs have begun
			{
				num_pids++;
				pids_to_wait = (pid_t *)realloc(pids_to_wait, sizeof(pid_t) * num_pids);
				pids_to_wait[num_pids - 1] = pids[x];
			}
			
			destroy_argv(argv2, argc2);
		}
		else {
			assert(false); // should only be one of the three types
		}
		destroy_argv(argv, argc);
	}
	
	for (int i = 0; i < num_pids; i++) //here the parallel jobs wait to all finish
	{
		waitpid(pids_to_wait[i], NULL, 0);
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
