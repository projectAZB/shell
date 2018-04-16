//
//  jobs.h
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef jobs_h
#define jobs_h

#include "job.h"

enum jobs_mode {
	parallel_m,
	serial_m
};

typedef enum jobs_mode jobs_mode;

struct jobs {
	jobs_mode jobs_mode;
	job_handle * jobs;
	size_t num_jobs;
};

typedef struct jobs jobs;

typedef jobs * jobs_handle;

jobs_handle create_jobs_handle(jobs_mode jobs_mode, job_handle * jobs, size_t num_jobs);

void destroy_jobs_handle(jobs_handle jobs_handle);

#endif
