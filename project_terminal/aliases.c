#include "functions.h"
#include "aliases.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

void print_all_aliases(aliases* alias_arr, int* num_of_aliases) {
    for (int i=0 ; i<*num_of_aliases ; i++) {
        printf("alias_arr[%d].tag =### %s\n", i, alias_arr[i].tag);
        for (int j=0 ; j<alias_arr[i].num_of_alias_args+1 ; j++) {
            printf("alias_arr[%d].m_cmd[%d] = %s\n", i, j, alias_arr[i].m_cmd[j]);
        }
        printf("alias_arr[%d].num_of_alias_args = %d\n", i, alias_arr[i].num_of_alias_args);
        printf("\n\n");
    }
}


void add_alias(aliases** alias_arr, char* pipes[MAX_PIPES][MAX_ARGS], int* num_of_aliases, command* commands, int* num_of_piped_commands) {
    int k; // alias_arr->args position counter
    // Check if the requested alias already exists -> dont add it
    for (int i=0 ; i<*num_of_aliases ; i++) {
        if (strcmp((*alias_arr)[i].tag, pipes[*num_of_piped_commands-1][1]) == 0) {
            printf("An alias with name: \"%s\" already exists\n", (*alias_arr)[i].tag);
            return;
        }
    }
    (*num_of_aliases)++;
    // printf("*num_of_piped_commands-1 = %d\n", *num_of_piped_commands-1); // = number of pipes 
    *alias_arr = (aliases*)realloc(*alias_arr, (*num_of_aliases) * sizeof(aliases));

    // Tag
    if (strcmp(pipes[0][1], "\0") == 0) {
        printf("Invalid alias creation. Usage: createalias [alias_name] \"[command]\"\n");
        return;
    }
    else {
        strcpy((*alias_arr)[*num_of_aliases-1].tag, pipes[0][1]); // pipes: [0] createalias , [1] tag_of_alias , [2] command , [3...] args 
    }

    // Args
    k=0; // iterate m_cmd
    for (int i=0 ; i<*num_of_piped_commands ; i++) { // iterate pipes[] to fill alias_arr[].m_cmd[]s
        if (i==0) { // we have to pass create+tag and go on from 3rd position
            for (int j=0 ; j<commands[i].num_of_args-1 ; j++) { /* -1 only for the first piped commands that include the "createalias" */
                (*alias_arr)[*num_of_aliases-1].m_cmd[k] = malloc(sizeof(char) * MAX_WORD_LENGTH);
                strcpy((*alias_arr)[*num_of_aliases-1].m_cmd[k], pipes[i][j+2]); // +1 avoid "createalias" & +1 avoid tag
                k++;
            }
        }
        else {
            for (int j=0 ; j<commands[i].num_of_args+1 ; j++) {
                (*alias_arr)[*num_of_aliases-1].m_cmd[k] = malloc(sizeof(char) * MAX_WORD_LENGTH);
                strcpy((*alias_arr)[*num_of_aliases-1].m_cmd[k], pipes[i][j]);
                k++;
            }
        }
        // Every time i increases -> it means there is a pipe -> place it in the next free space in the arryas.m_cmd section
        if (i < *num_of_piped_commands-1) { // do it only inbetween the commands, where it actually was before strtok()
            (*alias_arr)[*num_of_aliases-1].m_cmd[k] = malloc(sizeof(char) * MAX_WORD_LENGTH);
            strcpy((*alias_arr)[*num_of_aliases-1].m_cmd[k], "|");
            k++;
        }
    }

    // Num_of_args
    (*alias_arr)[*num_of_aliases-1].num_of_alias_args = k-1;

    // Remove the " just before the command     // works like: "ls -> ls
    int len = strlen((*alias_arr)[*num_of_aliases-1].m_cmd[0]);
    for (int i=0 ; i<len ; i++) {
        (*alias_arr)[*num_of_aliases-1].m_cmd[0][i] = (*alias_arr)[*num_of_aliases-1].m_cmd[0][i+1];
    }

    // Remove the " from the last argument string, just like the first argument
    int temp = (*alias_arr)[*num_of_aliases-1].num_of_alias_args;
    len = strlen((*alias_arr)[*num_of_aliases-1].m_cmd[temp]); // len of the last
    (*alias_arr)[*num_of_aliases-1].m_cmd[temp][len-1] = '\0';

    // For debugging
    // print_all_aliases((*alias_arr), num_of_aliases);
}


void remove_alias(char* alias_to_be_removed, aliases* alias_arr, int* num_of_aliases) {
    for (int k=0 ; k < *num_of_aliases ; k++) {
        int j=0; // declared here so that its value stays for the "while" part
        // We assume that the input is correct, like: destroyalias alias_name
        if (strcmp(alias_to_be_removed, alias_arr[k].tag) == 0) {
            if (k != *num_of_aliases-1) {
                // If its not the last alias
                // Move all the aliases after this one -> 1 position to the left (i start from the one i want to remove and copy on it the next one)
                for (int i=k ; i < *num_of_aliases ; i++) {
                    strcpy(alias_arr[k].tag, alias_arr[k+1].tag);
                    alias_arr[k].num_of_alias_args = alias_arr[k+1].num_of_alias_args;
                    // Copy all the args
                    // In case the next alias has less args - make the rest null
                    if (alias_arr[k].num_of_alias_args > alias_arr[k+1].num_of_alias_args) {    
                        while (j < alias_arr[k].num_of_alias_args) {
                            strcpy(alias_arr[k].m_cmd[j], "\0");
                            j++;
                        }
                    }
                    // Next alias has equal or more args - no problem
                    else {
                        for (j=0 ; j < alias_arr[k+1].num_of_alias_args+1 ; j++) {
                            strcpy(alias_arr[k].m_cmd[j], alias_arr[k+1].m_cmd[j]);
                        }
                    }
                    // print_all_aliases(alias_arr, num_of_aliases); // - for debugging
                }
            } else {
                // only 1 alias exists
                // Tag
                strcpy(alias_arr[k].tag, "\0");
                // Args
                for (j=0 ; j < alias_arr[k].num_of_alias_args+1 ; j++) {
                    strcpy(alias_arr[k].m_cmd[j], "\0");
                }
                // Num of args
                alias_arr[k].num_of_alias_args = 0;
            }
            (*num_of_aliases)--;
            printf("Alias removed\n");
            return; /*new*/
        }
    }
    printf("Alias not found\n");
}


void run_alias(command* commands, aliases* alias_arr, char** temp, int* curr_alias, char* curr_tag) {
    int local_num_of_piped_commands = 1;
    int pid; // id of the child that execute commands
    int fd[2];
    int read = 0;
    int write;

    int local_pipe_count;
    char* local_pipes[MAX_PIPES][MAX_ARGS];
    bool local_its_a_new_alias = false; // so that after parsing we know if it was just now that the alias has been added - so that we dont add it multiple times
    command* local_commands;

    char* local_cmd_args[MAX_ARGS];
    char local_cmd[MAX_CMD_LENGTH];
    int flag;   // so that i can start allocating the array to store the commands used by pipes ONLY IF there is a pipe in cmd line --- works only inside that for-loop
    int col;    /* for iteration on pipes[...][col] -> args of every command */
    int max_col;

    for (int i=0 ; i<MAX_PIPES ; i++) {
        for (int j=0 ; j<MAX_ARGS ; j++) {
            local_pipes[i][j] = malloc(sizeof(char) * 32);
            strcpy(local_pipes[i][j], "\0");
        }
    }

    strcpy(local_cmd, "\0");
    for (int i=0 ; i<alias_arr[*curr_alias].num_of_alias_args+1 ; i++) { 
        strcat(local_cmd, alias_arr[*curr_alias].m_cmd[i]);
        if (i < alias_arr[*curr_alias].num_of_alias_args)
            strcat(local_cmd, " ");
        else
            strcat(local_cmd, "\0");
    }

    local_cmd_args[0] = strtok(local_cmd, " ");
    for (int i=1 ; (local_cmd_args[i-1] != NULL) ; i++) {
        local_cmd_args[i] = strtok(NULL, " ");
    }

    // Check for exit command - SOOOS xreiazetai na grapsw tosa exit ~ osa alias exw kanei execute nmzw + meta petaei Seg - na to dw
    if (local_cmd_args[0] != NULL && strcmp(local_cmd_args[0], "exit") == 0) {
        exit(0);
    }

    if (local_cmd_args[0] != NULL && strcmp(local_cmd_args[0], "cd") == 0) {
        chdir(local_cmd_args[1]);
        printf("cd - current folder changed\n");
    }

    local_commands = (command*)malloc(sizeof(command));
    local_commands->num_of_args = 0;
    col=1;                                                                      // -> 2h 8esh tou pinaka -> 1o argument gia to antistoixo command(row)
    max_col = 0;                                                                // most right element of the array
    local_num_of_piped_commands = -1;                                           // wste na ginei 0 sto prwto ++num_of...
    flag = 1;
    for (int i = 0; local_cmd_args[i] != NULL; i++) {
        if (flag==1) {
            strcpy(local_pipes[++(local_num_of_piped_commands)][0], local_cmd_args[i]);
            flag = 0;                                                           // meta apo command dn ginetai na uparxei allo xwris pipe, klp
                                                                                // so that the we show that its the 1st arg and dont use extra blank spaces
            col = 1;
        
            local_commands = (command*)realloc(local_commands, (local_num_of_piped_commands+1) * sizeof(command)); // sos ta dereferencing sta orismata tis realloc
            strcpy(local_commands[local_num_of_piped_commands].name, local_cmd_args[i]);
            local_commands[local_num_of_piped_commands].num_of_args = 0;
        }
        else {
            if (strcmp(local_cmd_args[i], "|") == 0) {
                flag = 1;                                                       // next token must be a command and not an argument
                local_pipe_count += 1;
            }
            else {
                if (col > max_col) max_col = col;
                local_commands[local_num_of_piped_commands].num_of_args++;
                strcpy(local_pipes[local_num_of_piped_commands][col++], local_cmd_args[i]);
                flag=0;
            }
        }
    }

    local_num_of_piped_commands += 1;
    for (int i=0 ; i<(local_num_of_piped_commands) ; i++) {
        for (int j=0 ; j<max_col+1 ; j++) {
        }
    }

    // Execute command in sequence, piping output to input of the next command
    for (int i=0 ; i<local_num_of_piped_commands ; i++) {
       
        if (i < local_num_of_piped_commands-1) {                                            // fi there is at least 1 pipe in command
            if (pipe(fd) == -1) {                                                           // creates num_of_piped_commands-1 pipes
                printf("Error creating pipe\n");
                exit(1);
                // return;
            }
            else {
                // printf("pipe no.%d is created\n", i+1);
                write = fd[1];
            }
        } else {
            write = 1;
        }

        // Fork to execut the command
        pid = fork();
        if (pid == -1) {
            printf("Error creating process!\n");
            return;
        }
        if (pid == 0) {
            // Child Proccess
            // If there is >= 1 pipes -> redirect input to previous pipe or stdin
            if (local_pipe_count) {
                if (read != 0) {
                    //dup2(int oldfd, int newfd)
                    /*
                        The dup2() system call performs the same task as dup(), but
                        instead of using the lowest-numbered unused file descriptor, it
                        uses the file descriptor number specified in newfd.  In other
                        words, the file descriptor newfd is adjusted so that it now
                        refers to the same open file description as oldfd.
                    */
                    if (dup2(read, 0) == -1) {
                        printf("Error with dup2 - stdin N\n");
                    }
                    close(read);
                }
                if (write != 1) {
                    if (dup2(write, 1) == -1) {
                        printf("Error with dup2 - stdin N\n");
                    }
                    close(write);
                    close(fd[0]);
                }
            }

            if (!local_its_a_new_alias) {
                temp = (char**)malloc((local_commands[i].num_of_args+2) * sizeof(char*)); // +1 so that we place null at the end for execvp to work (i dont do that yet) & +1 for the command
                for (int j=0 ; j<local_commands[i].num_of_args+1 ; j++) { // + 1 cause its num of args + the command
                    temp[j] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
                    strcpy(temp[j], local_pipes[i][j]);
                }
                temp[local_commands[i].num_of_args+1] = NULL;

                if (execvp(temp[0], temp) == -1) {
                    printf("%s Command not found.\n", local_pipes[i][0]); // this must not get printed every time
                    continue;
                }
            }
            else {
                local_its_a_new_alias = false;
            }
            break;
        }
        else if (pid > 0) {
            // Parent Process
            if (write != 1) {
                close(write);
            }
            if (read != 0) {
                close(read);
            }
            read = fd[0];
            
            // Wait for children
            for (int i=0 ; i<local_num_of_piped_commands ; i++) {
                wait(NULL);
            }
        }

    }
}