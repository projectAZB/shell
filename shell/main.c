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

int main(int argc, const char * argv[]) {
	
	if (argc != 1) {
		char error_message[30] = "An error has occurred\n";
		write(STDOUT_FILENO, error_message, 30);
	}
	
	shell_handle shell = create_shell();
	start_shell(shell);
	destroy_shell(shell);
	
	return 0;
}
