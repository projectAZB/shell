CC = gcc
CFLAGS = -pedantic -std=gnu99 -Wall
DEBUG_FLAGS = -g $(CFLAGS)
VALGRIND_FLAGS = valgrind --show-reachable=yes --leak-check=full

mysh: main.c constants.h shell.h shell.c job_parser.c job_parser.h jobs.c jobs.h job.h job.c parser_helper.c executor.h executor.c\
	built_in.h built_in.c error.h error.c
	$(CC) $(DEBUG_FLAGS) main.c shell.c job_parser.c jobs.c job.c parser_helper.c executor.c error.c built_in.c -o mysh

vs : mysh
	$(VALGRIND_FLAGS) ./mysh

parser_test: parser_test_main.c job_parser.h job_parser.c jobs.c jobs.h job.h job.c constants.h parser_helper.c executor.h executor.c
	$(CC) $(DEBUG_FLAGS) job_parser.c parser_test_main.c jobs.c job.c executor.c parser_helper.c -o parser_test

vpt : parser_test
	$(VALGRIND_FLAGS) ./parser_test

.PHONY: clean
clean:
	rm -rf *.txt *.dSYM mysh parser_test
