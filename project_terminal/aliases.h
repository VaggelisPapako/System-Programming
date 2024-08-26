#ifndef ALIASES_H
#define ALIASES_H

#define MAX_CMD_LENGTH 512
#define MAX_ARGS 64
#define MAX_PIPES 64
#define MAX_WORD_LENGTH 64

#define MAX_ALIASES 100

#define MAX_HISTORY 20

#define READ 0
#define WRITE 1

typedef struct {
    char tag[MAX_WORD_LENGTH]; // alias name             px // ll
    char* m_cmd[MAX_ARGS]; // actual command                // "ls" "-la"
    int num_of_alias_args; // args of the actual command    // 1 oe 2?
} aliases;

typedef struct {
    char name[MAX_WORD_LENGTH];
    int num_of_args;
} command;


void add_alias(aliases** alias_arr, char* pipes[MAX_PIPES][MAX_ARGS], int* num_of_aliases, command* commands, int* num_of_piped_commands);
void remove_alias(char* alias_to_be_removed, aliases* alias_arr, int* num_of_aliases);
void run_alias(command* commands, aliases* alias_arr, char** temp, int* curr_alias, char* curr_tag);
void print_all_aliases(aliases* alias_arr, int* num_of_aliases);

#endif