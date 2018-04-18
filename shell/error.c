//
//  error.c
//  shell
//
//  Created by Adam on 4/18/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "error.h"
#include <unistd.h>

void print_error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDOUT_FILENO, error_message, 30);
}
