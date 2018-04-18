//
//  built_in.c
//  shell
//
//  Created by Adam on 4/18/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "built_in.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"

void run_quit() {
	exit(EXIT_SUCCESS);
}

void run_built_in_command_with_args(char ** argv)
{
	char * command_string = argv[0];
	if (strcmp(QUIT, command_string) == 0) {
		run_quit();
	}
	else if (strcmp(PWD, command_string) == 0) {
		return;
	}
	else if (strcmp(CD, command_string) == 0) {
		return;
	}
	else {
		assert(false);
	}
}
