//
//  jobs.c
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "jobs.h"

jobs_handle create_jobs_handle(jobs_mode jobs_mode, job_handle * jobs, size_t num_jobs)
{
	jobs_handle new = (jobs_handle)malloc(sizeof(*new));
	new->jobs_mode = jobs_mode;
	new->jobs = (job_handle *)malloc(sizeof(*(new->jobs)) * num_jobs);
	for (int i = 0; i < num_jobs; i++)
	{
		new->jobs[i] = copy_job(jobs[i]);
	}
	new->num_jobs = num_jobs;
	return new;
}

void destroy_jobs_handle(jobs_handle jobs_handle)
{
	for (int i = 0; i < jobs_handle->num_jobs; i++)
	{
		destroy_job(jobs_handle->jobs[i]);
	}
	free(jobs_handle->jobs);
	free(jobs_handle);
}
