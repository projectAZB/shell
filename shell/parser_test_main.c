//
//  parser_test_main.c
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "job_parser.h"
#include "constants.h"

void command_test()
{
	job_parser_handle cp1 = create_job_parser("cd | ls\n");
	jobs_handle jh1 = run_job_parser(cp1);
	assert(jh1->num_jobs == 1);
	assert(strcmp("cd", jh1->jobs[0]->command_strings[0]) == 0);
	assert(argc_for_job(jh1->jobs[0], 0));
	char ** argv1 = argv_for_job(jh1->jobs[0], 0); //the second is the command_index into the job
	assert(strcmp(argv1[0], "cd") == 0);
	free(argv1);
	assert(jh1->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp("ls", jh1->jobs[0]->command_strings[1]) == 0);
	assert(jh1->jobs[0]->command_types[1] == external_t);
	assert(argc_for_job(jh1->jobs[0], 1));
	argv1 = argv_for_job(jh1->jobs[0], 1);
	assert(strcmp(argv1[0], "ls") == 0);
	free(argv1);
	destroy_jobs_handle(jh1);
	destroy_job_parser(cp1);
	
	job_parser_handle cp2 = create_job_parser("cd; ls\n");
	jobs_handle jh2 = run_job_parser(cp2);
	assert(jh2->num_jobs == 2);
	assert(strcmp("cd", jh2->jobs[0]->command_strings[0]) == 0);
	assert(jh2->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp("ls", jh2->jobs[1]->command_strings[0]) == 0);
	assert(jh2->jobs[1]->command_types[0] == external_t);
	assert(argc_for_job(jh2->jobs[1], 0) == 1);
	char ** argv2 = argv_for_job(jh2->jobs[1], 0);
	assert(strcmp(argv2[0], "ls") == 0);
	free(argv2);
	destroy_jobs_handle(jh2);
	destroy_job_parser(cp2);
	
	job_parser_handle cp3 = create_job_parser("cd+\n");
	jobs_handle jh3 = run_job_parser(cp3);
	assert(jh3->num_jobs == 1);
	assert(strcmp("cd", jh3->jobs[0]->command_strings[0]) == 0);
	assert(jh3->jobs[0]->command_types[0] == built_in_t);
	assert(argc_for_job(jh3->jobs[0], 0) == 1);
	char ** argv3 = argv_for_job(jh3->jobs[0], 0);
	assert(strcmp(argv3[0], "cd") == 0);
	free(argv3);
	destroy_jobs_handle(jh3);
	destroy_job_parser(cp3);
	
	job_parser_handle cp4 = create_job_parser("cd     \n");
	jobs_handle jh4 = run_job_parser(cp4);
	assert(jh4->num_jobs == 1);
	assert(strcmp("cd", jh4->jobs[0]->command_strings[0]) == 0);
	assert(jh4->jobs[0]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh4);
	destroy_job_parser(cp4);
	
	job_parser_handle cp5 = create_job_parser("cd;     \n");
	jobs_handle jh5 = run_job_parser(cp5);
	assert(jh5->num_jobs == 1);
	assert(strcmp("cd", jh5->jobs[0]->command_strings[0]) == 0);
	assert(jh5->jobs[0]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh5);
	destroy_job_parser(cp5);
	
	job_parser_handle cp6 = create_job_parser("   cd+  ls + cd   \n");
	jobs_handle jh6 = run_job_parser(cp6);
	assert(jh6->num_jobs == 3);
	assert(strcmp("cd", jh6->jobs[0]->command_strings[0]) == 0);
	assert(jh6->jobs[0]->command_types[0] == built_in_t);
	assert(jh6->jobs[0]->job_type == normal_t);
	assert(strcmp("ls", jh6->jobs[1]->command_strings[0]) == 0);
	assert(jh6->jobs[1]->job_type == normal_t);
	assert(jh6->jobs[1]->command_types[0] == external_t);
	assert(strcmp("cd", jh6->jobs[2]->command_strings[0]) == 0);
	assert(jh6->jobs[2]->job_type == normal_t);
	assert(jh6->jobs[2]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh6);
	destroy_job_parser(cp6);
	
	job_parser_handle cp7 = create_job_parser("   cd;  ls > grep; cd   \n");
	jobs_handle jh7 = run_job_parser(cp7);
	assert(jh7->num_jobs == 3);
	assert(strcmp("cd", jh7->jobs[0]->command_strings[0]) == 0);
	assert(jh7->jobs[0]->job_type == normal_t);
	assert(jh7->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp("ls", jh7->jobs[1]->command_strings[0]) == 0);
	assert(jh7->jobs[1]->job_type == redirection_t);
	assert(jh7->jobs[1]->command_types[0] == external_t);
	assert(strcmp("grep", jh7->jobs[1]->command_strings[1]) == 0);
	assert(jh7->jobs[1]->command_types[1] == external_t);
	assert(strcmp("cd", jh7->jobs[2]->command_strings[0]) == 0);
	assert(jh7->jobs[2]->job_type == normal_t);
	assert(jh7->jobs[2]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh7);
	destroy_job_parser(cp7);
	
	job_parser_handle cp8 = create_job_parser("cd;cd;cd;ls;grep;grep\n");
	jobs_handle jh8 = run_job_parser(cp8);
	assert(jh8->num_jobs == 6);
	assert(strcmp("cd", jh8->jobs[0]->command_strings[0]) == 0);
	assert(strcmp("cd", jh8->jobs[1]->command_strings[0]) == 0);
	assert(strcmp("cd", jh8->jobs[2]->command_strings[0]) == 0);
	assert(strcmp("ls", jh8->jobs[3]->command_strings[0]) == 0);
	assert(strcmp("grep", jh8->jobs[4]->command_strings[0]) == 0);
	assert(strcmp("grep", jh8->jobs[5]->command_strings[0]) == 0);
	destroy_jobs_handle(jh8);
	destroy_job_parser(cp8);
}

void arg_test()
{
	job_parser_handle cp1 = create_job_parser("ls -a\n");
	jobs_handle jh1 = run_job_parser(cp1);
	assert(jh1->num_jobs == 1);
	assert(strcmp("ls", jh1->jobs[0]->command_strings[0]) == 0);
	assert(jh1->jobs[0]->command_types[0] == external_t);
	assert(jh1->jobs[0]->arg_counts[0] == 1);
	assert(strcmp("-a", jh1->jobs[0]->args[0][0]) == 0);
	assert(argc_for_job(jh1->jobs[0], 0) == 2);
	char ** argv1 = argv_for_job(jh1->jobs[0], 0);
	assert(strcmp(argv1[0], "ls") == 0);
	assert(strcmp(argv1[1], "-a") == 0);
	free(argv1);
	destroy_jobs_handle(jh1);
	destroy_job_parser(cp1);
	
	job_parser_handle cp2 = create_job_parser("cd; ls\n");
	jobs_handle jh2 = run_job_parser(cp2);
	assert(jh2->num_jobs == 2);
	assert(strcmp("cd", jh2->jobs[0]->command_strings[0]) == 0);
	assert(jh2->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp("ls", jh2->jobs[1]->command_strings[0]) == 0);
	assert(jh2->jobs[1]->command_types[0] == external_t);
	destroy_jobs_handle(jh2);
	destroy_job_parser(cp2);
	
	job_parser_handle cp3 = create_job_parser("cd;;;;;;;grep \".h\";ls;grep\n");
	jobs_handle jh3 = run_job_parser(cp3);
	assert(jh3->num_jobs == 4);
	assert(strcmp("cd", jh3->jobs[0]->command_strings[0]) == 0);
	assert(jh3->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp(jh3->jobs[1]->command_strings[0], "grep") == 0);
	assert(jh3->jobs[1]->arg_counts[0] == 1);
	assert(argc_for_job(jh3->jobs[0], 0) == 1);
	assert(argc_for_job(jh3->jobs[1], 0) == 2);
	assert(argc_for_job(jh3->jobs[2], 0) == 1);
	assert(argc_for_job(jh3->jobs[3], 0) == 1);
	
	char ** argv3 = argv_for_job(jh3->jobs[1], 0);
	assert(strcmp(argv3[0], "grep") == 0);
	assert(strcmp(argv3[1], "\".h\"") == 0);
	free(argv3);
	
	destroy_jobs_handle(jh3);
	destroy_job_parser(cp3);
	
	job_parser_handle cp4 = create_job_parser("cd     \n");
	jobs_handle jh4 = run_job_parser(cp4);
	assert(jh4->num_jobs == 1);
	assert(strcmp("cd", jh4->jobs[0]->command_strings[0]) == 0);
	assert(jh4->jobs[0]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh4);
	destroy_job_parser(cp4);
	
	job_parser_handle cp5 = create_job_parser("cd;;;;;     \n");
	jobs_handle jh5 = run_job_parser(cp5);
	assert(jh5->num_jobs == 1);
	assert(strcmp("cd", jh5->jobs[0]->command_strings[0]) == 0);
	assert(jh5->jobs[0]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh5);
	destroy_job_parser(cp5);
	
	job_parser_handle cp6 = create_job_parser("   cd+  ls -a -a -a -a -a -a -a | grep -a \".c\"+ cd   \n");
	jobs_handle jh6 = run_job_parser(cp6);
	assert(jh6->num_jobs == 3);
	assert(strcmp("cd", jh6->jobs[0]->command_strings[0]) == 0);
	assert(jh6->jobs[0]->command_types[0] == built_in_t);
	assert(jh6->jobs[0]->job_type == normal_t);
	assert(strcmp("ls", jh6->jobs[1]->command_strings[0]) == 0);
	assert(jh6->jobs[1]->arg_counts[0] == 7);
	assert(jh6->jobs[1]->arg_counts[1] == 2);
	assert(argc_for_job(jh6->jobs[1], 0) == 8);
	assert(argc_for_job(jh6->jobs[1], 1) == 3);
	for (int i = 0; i < jh6->jobs[1]->arg_counts[0]; i++)
	{
		assert(strcmp("-a", jh6->jobs[1]->args[0][i]) == 0);
	}
	assert(jh6->jobs[1]->job_type == piped_t);
	assert(jh6->jobs[1]->command_types[0] == external_t);
	assert(jh6->jobs[1]->arg_counts[1] == 2);
	assert(strcmp("cd", jh6->jobs[2]->command_strings[0]) == 0);
	assert(jh6->jobs[2]->job_type == normal_t);
	assert(jh6->jobs[2]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh6);
	destroy_job_parser(cp6);
	
	job_parser_handle cp7 = create_job_parser("   cd -ab; ls -la /tmp > grep \".c\"; cd   \n");
	jobs_handle jh7 = run_job_parser(cp7);
	assert(jh7->num_jobs == 3);
	assert(strcmp("cd", jh7->jobs[0]->command_strings[0]) == 0);
	assert(jh7->jobs[0]->job_type == normal_t);
	assert(jh7->jobs[0]->arg_counts[0] == 1);
	assert(strcmp("-ab", jh7->jobs[0]->args[0][0]) == 0);
	assert(jh7->jobs[0]->command_types[0] == built_in_t);
	assert(strcmp("ls", jh7->jobs[1]->command_strings[0]) == 0);
	assert(strcmp("-la", jh7->jobs[1]->args[0][0]) == 0);
	assert(strcmp("/tmp", jh7->jobs[1]->args[0][1]) == 0);
	assert(jh7->jobs[1]->job_type == redirection_t);
	assert(jh7->jobs[1]->command_types[0] == external_t);
	assert(strcmp("grep", jh7->jobs[1]->command_strings[1]) == 0);
	assert(jh7->jobs[1]->command_types[1] == external_t);
	assert(strcmp("cd", jh7->jobs[2]->command_strings[0]) == 0);
	assert(jh7->jobs[2]->job_type == normal_t);
	assert(jh7->jobs[2]->command_types[0] == built_in_t);
	destroy_jobs_handle(jh7);
	destroy_job_parser(cp7);
	
	job_parser_handle cp8 = create_job_parser("cd /dir -a;cd;cd;ls -a;grep \".c\";grep\n");
	jobs_handle jh8 = run_job_parser(cp8);
	assert(jh8->num_jobs == 6);
	assert(strcmp("cd", jh8->jobs[0]->command_strings[0]) == 0);
	assert(strcmp("/dir", jh8->jobs[0]->args[0][0]) == 0);
	assert(strcmp("-a", jh8->jobs[0]->args[0][1]) == 0);
	assert(strcmp("cd", jh8->jobs[1]->command_strings[0]) == 0);
	assert(strcmp("cd", jh8->jobs[2]->command_strings[0]) == 0);
	assert(strcmp("ls", jh8->jobs[3]->command_strings[0]) == 0);
	assert(strcmp("grep", jh8->jobs[4]->command_strings[0]) == 0);
	assert(strcmp("\".c\"", jh8->jobs[4]->args[0][0]) == 0);
	assert(strcmp("grep", jh8->jobs[5]->command_strings[0]) == 0);
	destroy_jobs_handle(jh8);
	destroy_job_parser(cp8);
}

void mixed_test() {
	job_parser_handle cp1 = create_job_parser("cd ; ls+\n");
	jobs_handle jh1 = run_job_parser(cp1);
	assert(jh1 == NULL);
	destroy_job_parser(cp1);
	
	job_parser_handle cp2 = create_job_parser("cd;;+++ls\n");
	jobs_handle jh2 = run_job_parser(cp2);
	assert(jh2 == NULL);
	destroy_job_parser(cp2);
	
	job_parser_handle cp3 = create_job_parser("cd;;+++\n");
	jobs_handle jh3 = run_job_parser(cp3);
	assert(jh3 == NULL);
	destroy_job_parser(cp3);
}

int main(int argc, const char * argv[]) {
	
	command_test();
	arg_test();
	mixed_test();
	
	return 0;
}
