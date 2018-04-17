//
//  executor.h
//  shell
//
//  Created by Adam on 4/17/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef executor_h
#define executor_h

#include "jobs.h"

struct executor;

typedef struct executor executor;

typedef executor * executor_handle;

executor_handle create_executor(jobs_handle jobs);

void run(executor_handle executor);

void destroy_executor(executor_handle executor);

#endif 
