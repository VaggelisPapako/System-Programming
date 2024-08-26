#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "history.h"
#include "aliases.h"
#include <stdbool.h>


#define MAX_CMD_LENGTH 512
#define MAX_ARGS 64
#define MAX_PIPES 64
#define MAX_WORD_LENGTH 64

#define MAX_ALIASES 100

#define MAX_HISTORY 20

#define READ 0
#define WRITE 1

// void parse_input(char* , command** , aliases** , int* , bool* , reverse_stack* , int* , int* );
// void execute_commands(int* , int* , int* , int* , bool* , int* , command* , aliases* , char* );

void parse_input(char* (*pipes)[MAX_PIPES][MAX_ARGS], command** commands, aliases** alias_arr, int* num_of_aliases, bool* its_a_new_alias, reverse_stack* history, int* num_of_piped_commands, int* pipe_count, int* file, int* stdin_copy, int* stdout_copy);
void execute_commands(int* num_of_piped_commands, int* read, int* write, int* pipe_count, bool* its_a_new_alias, int* num_of_aliases, command* commands, aliases* alias_arr, char* pipes[MAX_PIPES][MAX_ARGS], int* file);

#endif