//
//  executor.c
//  shell
//
//  Created by Adam on 4/17/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "executor.h"

struct executor {
	jobs_handle jobs;
}

executor_handle create_executor(jobs_handle jobs)
{
	executor_handle new = (executor_handle)malloc(sizeof(*new));
	new->jobs = jobs;
	return new;
}

void run(executor_handle executor)
{
	
}

void destroy_executor(executor_handle executor)
{
	free(executor);
}
