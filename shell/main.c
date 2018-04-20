//
//  main.c
//  shell
//
//  Created by Adam on 4/11/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "shell.h"
#include "error.h"

int main(int argc, const char * argv[]) {
	
	if (argc > 2) {
		print_error();
		exit(EXIT_FAILURE);
	}
	else if (argc == 2) { //handle input file case
		shell_handle shell = create_shell_with_filename(argv[1]);
		start_shell(shell);
		destroy_shell(shell);
	}
	else { //argc == 1
		shell_handle shell = create_shell();
		start_shell(shell);
		destroy_shell(shell);
	}
	
	return 0;
}
