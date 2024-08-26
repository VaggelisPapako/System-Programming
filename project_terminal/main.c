#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "functions.h"
#include "aliases.h"    // an einai prin to functions.h bgazei errors
#include "history.h"

int main(int argc, char* argv[]) {
    
        int num_of_piped_commands, pipe_count/*counter of pipes*/;
       
        reverse_stack history;

        aliases* alias_arr;
        int num_of_aliases = 0; // works also as a pointer to the first empty spot in the array so that we dont waste time searching for it every time
        bool its_a_new_alias = false; // so that after parsing we know if it was just now that the alias has been added - so that we dont add it multiple times

        command* commands; // array of commands "objects"
        char* pipes[MAX_PIPES][MAX_ARGS]; // used for pipes - stores every seperate command(column0)+ITSarguments(column1) in a single row


        init_rs(&history);



    while (1) {
        
        int file, stdin_copy, stdout_copy;
        int read = 0;
        int write;

        for (int i=0 ; i<MAX_PIPES ; i++) {
            for (int j=0 ; j<MAX_ARGS ; j++) {
                pipes[i][j] = malloc(sizeof(char) * 32);
                strcpy(pipes[i][j], "\0");
            }
        }

        printf("\n");
        printf("in-mysh-now:> ");
        fflush(stdout);


        // Parse command into arguments
        parse_input(&pipes, &commands, &alias_arr, &num_of_aliases, &its_a_new_alias, &history, &num_of_piped_commands, &pipe_count, &file, &stdin_copy, &stdout_copy);

        // If its an alias created - handle it only this loop and dont execut anything this time
        if (its_a_new_alias) {
            add_alias(&alias_arr, pipes, &num_of_aliases, commands, &num_of_piped_commands);           
            its_a_new_alias = false;
            continue;
        }

        // Print all aliases - for debugging
        print_all_aliases(alias_arr, &num_of_aliases); 
        
        execute_commands(&num_of_piped_commands, &read, &write, &pipe_count, &its_a_new_alias, &num_of_aliases, commands, alias_arr, pipes, &file);

    }

    // clean_rs(&history);
    
    // printf("akrivws meta to while!\n");


    return 0;
}