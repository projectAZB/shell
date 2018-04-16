//
//  constants.h
//  shell
//
//  Created by Adam on 4/12/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef constants_h
#define constants_h

#define QUIT "quit"
#define PWD "pwd"
#define CD "cd"

#define SEMICOLON ';'
#define PLUS '+'
#define REDIRECT '>'
#define PIPE '|'
#define NEWLINE '\n'
#define NULL_TERM '\0'
#define SPACE ' '
#define TAB '\t'
#define C_RETURN '\r'

enum job_type {
	normal_t,
	redirection_t,
	piped_t
};

typedef enum job_type job_type;

#endif
