//
//  parser_helper.c
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "parser_helper.h"

#include "constants.h"

//end line
bool is_line_end(char c)
{
	return c == NEWLINE;
}

//ignored space
bool is_ignored_space(char c)
{
	return c == SPACE || c == TAB || c == C_RETURN;
}

//semicolon plus, or newline
bool is_job_end(char c)
{
	return c == SEMICOLON || c == PLUS || is_line_end(c);
}

bool is_pipe_or_redirect(char c)
{
	return c == PIPE || c == REDIRECT;
}

//pipe or redirection
bool is_job_segment_end(char c)
{
	return is_pipe_or_redirect(c) || is_job_end(c);
}
