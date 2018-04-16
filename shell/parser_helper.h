//
//  parser_helper.h
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef parser_helper_h
#define parser_helper_h

#include <stdbool.h>

//end line
bool is_line_end(char c);
//ignored space
bool is_ignored_space(char c);
//semicolon, plus, or newline
bool is_job_end(char c);
//pipe or redirection or job end
bool is_job_segment_end(char c);

bool is_pipe_or_redirect(char c);

#endif
