//
//  error.c
//  shell
//
//  Created by Adam on 4/18/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "error.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void print_error_and_exit()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(EXIT_FAILURE);
}
