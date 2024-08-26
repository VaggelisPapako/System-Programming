#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "functions.h"





void parse_input(char* (*pipes)[MAX_PIPES][MAX_ARGS], command** commands, aliases** alias_arr, int* num_of_aliases, bool* its_a_new_alias, reverse_stack* history, int* num_of_piped_commands, int* pipe_count, int* file, int* stdin_copy, int* stdout_copy) {
    char* cmd_args[MAX_ARGS];   // stores the seperated command+arguments from the user (pos[0]command + pos[1, 2, ...]args)
    char cmd[MAX_CMD_LENGTH];   // stores the command from the user (whole line)
    *pipe_count = 0;            // so that we know if there is a pipe, the other flags cannot be used for this
    int flag;                   // so that i can start allocating the array to store the commands used by pipes ONLY IF there is a pipe in cmd line --- works only inside that for-loop
    int col;                    /* for iteration on pipes[...][col] -> args of every command */
    int max_col;                /* most right element of the array */
    int len;
    
    char* temp_redir[MAX_ARGS]; //  for commands used with redirection(s)
    char** command_redir;       //  same
    bool is_out;
    // bool is_in;
    bool is_append;
    int pos;


    fgets(cmd, MAX_CMD_LENGTH, stdin);                                          // we store the command from STDIN
    add_to_history(history, cmd);
    cmd[strlen(cmd)-1] = '\0';                                                  // Remove newline character
    
    cmd_args[0] = strtok(cmd, " ");
    for (int i=1 ; /*(i < MAX_ARGS-1) &&*/ (cmd_args[i-1] != NULL) ; i++) {
        cmd_args[i] = strtok(NULL, " ");
    }

    // This exists here so that we can execute commands like: myHistory 5 that point to same commands like: myHistory (num)
    // NEEDS A FLAG TO WORK
    // if (strcmp(cmd_args[0], "myHistory") == 0) {
    //     // If there is NO specific command request
    //     if (cmd_args[1] == NULL) {
    //         printf(" KALW PRINT HISTORY 3\n");
    //         print_history(history, -1);
    //     }
    //     else {
    //     // If there is a specific command request
    //         int temp = atoi(cmd_args[1]);
    //         printf(" KALW PRINT HISTORY 4\n");
    //         print_history(history, temp);
    //     }
    // }
    
    // Check for exit command - SOOOS xreiazetai na grapsw tosa exit ~ osa alias exw kanei execute nmzw? + meta petaei Seg - na to dw
    if (cmd_args[0] != NULL && strcmp(cmd_args[0], "exit") == 0) {
        exit(0);
    }

    if (cmd_args[0] != NULL && strcmp(cmd_args[0], "cd") == 0) {
        chdir(cmd_args[1]);
        printf("cd - current folder changed\n");
    }

    command_redir = NULL;

    len = 0;
    for (int i=0 ; cmd_args[i] != NULL ; i++) {
        temp_redir[i] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
        strcpy(temp_redir[i], cmd_args[i]);
        len++;
    }
     // in the end len = actual length + 1 

    for (int i=0 ; i<len ; i++) {

        if ((!strcmp(temp_redir[i], ">")) || (!strcmp(temp_redir[i], "<")) || (!strcmp(temp_redir[i], ">>"))) {
            is_out = strcmp(temp_redir[i], ">") || strcmp(temp_redir[i], ">>");
            is_append = strcmp(temp_redir[i], ">>");
            if (command_redir == NULL) {
                command_redir = (char**)malloc(MAX_ARGS * sizeof(char*));
                for (int j=0 ; j<i ; j++) {
                    command_redir[j] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
                    strcpy(command_redir[j], temp_redir[j]);
                    pos = i;
                }
            }

            // *stdin_copy = dup(0);
            // *stdout_copy = dup(1);
            // close(0);
            // close(1);
            
            if (i<len-1) { // if next element exists
                if (!is_append) {
                    *file = open(temp_redir[i+1], O_WRONLY | O_CREAT, 0666);
                }
                else {
                    *file = open(temp_redir[i+1], O_WRONLY | O_APPEND | O_CREAT, 0666);
                }
            }

            if (is_out) {
                // output redirection
                dup2(*file, STDOUT_FILENO);
            }
            else {
                dup2(*file, STDIN_FILENO);
            }
            for (int i=0 ; i<pos ; i++) {
                strcpy(cmd_args[i], command_redir[i]);
            }
            cmd_args[pos] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
            cmd_args[pos] = NULL;
        }

    }

    // Na checkarw an einai swsto to shmeio
    for (int j=0 ; j<pos ; j++) {
        free(command_redir[j]);
    }
    free(command_redir);

    for (int i=0 ; i<len ; i++) {
        free(temp_redir[i]);
    }
    
    // Check for pipes in command from user
    *commands = (command*)malloc(sizeof(command));
    (*commands)->num_of_args = 0;
    col=1;                                                                          // -> 2h 8esh tou pinaka -> 1o argument gia to antistoixo command(row)
    max_col = 0;                                                                    // keep the most right column of the array
    *num_of_piped_commands = -1;                                                    // wste na ginei 0 sto prwto ++num_of...
    flag = 1;
    for (int i = 0; cmd_args[i] != NULL; i++) {
        if (flag==1) {                                                              // flag=1 (starting value) means it a command and not an argument token
            strcpy((*pipes)[++(*num_of_piped_commands)][0], cmd_args[i]);
            flag = 0;                                                               // meta apo command dn ginetai na uparxei allo xwris pipe, klp
                                                                                    // so that the we show that its the 1st arg and dont use extra blank spaces
            col = 1;                                                                // reset ton pointer gia ta args - pleon mallon dn xreiazetai

            *commands = (command*)realloc(*commands, (*num_of_piped_commands+1) * sizeof(command)); // sos ta dereferencing sta orismata tis realloc
            strcpy((*commands)[*num_of_piped_commands].name, cmd_args[i]);
            (*commands)[*num_of_piped_commands].num_of_args = 0;

            if (strcmp(cmd_args[i], "createalias") == 0) {
                *its_a_new_alias = true; // raise flag to continue alias processing after the loop (so that all next args get parsed) 
            }
            
            if (strcmp(cmd_args[i], "destroyalias") == 0) {
                remove_alias(cmd_args[i+1], (*alias_arr), num_of_aliases);
                // Because i haven't read the alias name yet, i raise a flag so that i remove it as soon as i know
            }


            // if (strcmp(cmd_args[i], "bg") == 0) {} // ftiaxnw pipe k to bazw na grafei ekei???
            // if (strcmp(cmd_args[i], "fg") == 0) { // to bazw na diabazei apo to parapanw pipe


        }
        else { // flag=0 -> non-command
            if (strcmp(cmd_args[i], "|") == 0) {
                    flag = 1;                                                       // to epomeno token prepei na einai command k oxi argument
                    *pipe_count += 1;
            }
            // if (strcmp(cmd_args[i], ";") == 0) {                                 // dinei amesws meta epomenh entolh
            // 
            // }
            else {                                                                  // einai aplo argument token enos command
                if (col > max_col) max_col = col;                                   // kratame to dexiotero akro tou pinaka - pleon mallon dn xreiazetai
                (*commands)[*num_of_piped_commands].num_of_args++;
                strcpy((*pipes)[*num_of_piped_commands][col++], cmd_args[i]);       // -> filling pipes[i][0,1, ...]
                flag=0;                                                             // meta apo argument uparxei mono neo argument k oxi command - mporei na mhn xreiazetai
            }
        }
    }

    // FIX STDIN STDOUT EDW MALLON
    
    // Print the filled portion of pipes array - for debugging
    // Sthn ousia to num_of_piped_commands einai += 1 -> giauto tou dinoume ontws thn swsth timh pou exei
    *num_of_piped_commands += 1;
    for (int i=0 ; i<(*num_of_piped_commands) ; i++) {                              // +1 gt einai 0 = 8esh prwtou command, 1=2 commands total + 8esh 2ou command
        for (int j=0 ; j<max_col+1 ; j++) {
            // printf("###pipes[%d][%d] = %s\n", i, j, (*pipes)[i][j]);
        }
    }

}






















void execute_commands(int* num_of_piped_commands, int* read, int* write, int* pipe_count, bool* its_a_new_alias, int* num_of_aliases, command* commands, aliases* alias_arr, char* pipes[MAX_PIPES][MAX_ARGS], int* file) {
    int pid; // id of the child that execute commands
    char** temp; // for execvp args
    int fd[2];

    // Execute command in sequence, piping output to input of the next command
    for (int i=0 ; i<*num_of_piped_commands ; i++) {
        
        if (i < *num_of_piped_commands-1) {    
            if (pipe(fd) == -1) {                                    // creates num_of_piped_commands-1 pipes
                printf("Error creating pipe\n");
                exit(1);
            }
            else {
                printf("pipe no.%d is created\n", i+1);
                *write = fd[1];
            }
        } else {
            *write = 1;
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
            if (*pipe_count) {                                                   // could be pipe_count >= 1 -> exei hdh ginei elegxos 20 grammes panw -> na to dw
                if (*read != 0) {
                    //dup2(int oldfd, int newfd)
                    /*
                        The dup2() system call performs the same task as dup(), but
                        instead of using the lowest-numbered unused file descriptor, it
                        uses the file descriptor number specified in newfd.  In other
                        words, the file descriptor newfd is adjusted so that it now
                        refers to the same open file description as oldfd.
                    */
                    if (dup2(*read, 0) == -1) {
                        printf("Error with dup2 - stdin N\n");
                    }
                    close(*read);
                }
                if (*write != 1) {
                    if (dup2(*write, 1) == -1) {
                        printf("Error with dup2 - stdin N\n");
                    }
                    close(*write);
                    close(fd[0]);
                }
            }

            // If its a normal command or an alias execute it normally
            // Otherwise if its an alias creation dont exec anything
            if(!(*its_a_new_alias)) {
                // If its a command from aliases (not a creation)
                for (int k=0 ; k<*num_of_aliases ; k++) {
                    if (strcmp(commands[i].name, alias_arr[k].tag) == 0) {
                        run_alias(commands, alias_arr, temp, &k, alias_arr[k].tag);
                        continue;
                    }
                }
            
                temp = (char**)malloc((commands[i].num_of_args+2) * sizeof(char*)); // +1 so that we place null at the end for execvp to work (i dont do that yet) & +1 for the command
                for (int j=0 ; j<commands[i].num_of_args+1 ; j++) { // + 1 cause its num of args + the command
                    temp[j] = (char*)malloc(MAX_WORD_LENGTH * sizeof(char));
                    strcpy(temp[j], pipes[i][j]);
                    // printf("temp[%d] ======in exec======= %s\n", j, temp[j]);
                }
                temp[commands[i].num_of_args+1] = NULL;

                if (execvp(temp[0], temp) == -1) {

                    // printf("%s Command not found.\n", pipes[i][0]);
                    for (int j=0 ; j<commands[i].num_of_args+1 ; j++) { // + 1 cause its num of args + the command
                        free(temp[j]);
                    }
                    free(temp[commands[i].num_of_args+1]);
                    free(temp);
                    continue;
                }
                
                for (int j=0 ; j<commands[i].num_of_args+1 ; j++) { // + 1 cause its num of args + the command
                    free(temp[j]);
                }
                free(temp[commands[i].num_of_args+1]);
                free(temp);


            }
            else {
                *its_a_new_alias = false;   // show that we are done processing the alias and can execute next commands
            }

            break;  // so that children dont generate ^2
        }
        else if (pid > 0) {
            // Parent Process
            if (*write != 1) {
                close(*write);
            }
            if (*read != 0) {
                close(*read);
            }
            *read = fd[0];
            
            // Wait for children
            for (int i=0 ; i<*num_of_piped_commands ; i++) {
                wait(NULL);
            }

        }

    }
}