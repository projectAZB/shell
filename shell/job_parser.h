//
//  job_parser.h
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef job_parser_h
#define job_parser_h

#include "jobs.h"

struct job_parser {
	char * command_string;
};

typedef struct job_parser job_parser;

typedef job_parser * job_parser_handle;

job_parser_handle create_job_parser(char * command_string);

jobs_handle run_job_parser(job_parser_handle job_parser);

void destroy_job_parser(job_parser_handle job_parser);

#endif
