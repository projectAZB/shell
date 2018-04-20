//
//  shell.h
//  shell
//
//  Created by Adam on 4/11/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef shell_h
#define shell_h

enum shell_mode {
	normal_m,
	file_m
};

typedef enum shell_mode shell_mode;

struct shell;

typedef struct shell shell;

typedef shell * shell_handle;

shell_handle create_shell(void);
shell_handle create_shell_with_filename(const char * filename);
void start_shell(shell_handle shell);
void destroy_shell(shell_handle shell);

#endif
