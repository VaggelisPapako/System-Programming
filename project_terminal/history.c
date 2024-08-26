#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "history.h"

#define MAX_CMD_LENGTH 512
#define MAX_ARGS 64
#define MAX_PIPES 64
#define MAX_WORD_LENGTH 64

#define MAX_ALIASES 100

#define MAX_HISTORY 20

#define READ 0
#define WRITE 1


void init_rs(reverse_stack* rs) {
    rs->history = (char**)malloc(MAX_HISTORY * sizeof(char*));
    for (int i=0 ; i<MAX_HISTORY ; i++) {
        rs->history[i] = (char*)malloc(MAX_CMD_LENGTH * sizeof(char));
    }
    rs->curr_pos = 0;
}

void clean_rs(reverse_stack* rs) {
    for (int i=0 ; i<MAX_HISTORY ; i++) {
        free(rs->history[i]);
    }
    free(rs->history);
}

int isFull(reverse_stack* rs) {
    return rs->curr_pos == MAX_HISTORY;
}

int isEmpty(reverse_stack* rs) { // needed only for print_history
    return rs->curr_pos == 0;
}

void print_history(reverse_stack* rs, int num) {
    if (isEmpty(rs)) {
        // Basically it never gets here because MyHistory command always counts as one if its the first command of all
        printf("Command history is empty\n");
        return;
    }
    else {
        if (num == -1) {
            printf("Command History:\n");
            if (rs->curr_pos < MAX_HISTORY-1) {
                for(int i=0 ; i<rs->curr_pos ; i++){
                    printf("%d. %s\n", i+1, rs->history[i]);
                }
            }
            else {
                for(int i=0 ; i<MAX_HISTORY ; i++){
                    printf("%d. %s\n", i+1, rs->history[i]);
                }
            }
        }
        else {
            if (num <= rs->curr_pos) {
                printf("Command no.%d:\n", num);
                printf("%d. %s\n", num, rs->history[num-1]);
            }
            else {
                printf("You have entered less than %d commands\n", num);
            }
        }
    }
}

void add_to_history(reverse_stack* rs, char* cmd) {
    char* cmd_args[MAX_ARGS]; // just aux
    char* temp; // so that we dont ruin the actual cmd after strtok()
    if (isFull(rs)) {
        temp = rs->history[0]; // least recently used command
        // 1st to 2nd-to-last command -> shift right 1 position
        for (int i=0 ; i<MAX_HISTORY-1 ; i++) {
            // printf("rs->history[%d] WAS = %s\n", i, rs->history[i]);
            rs->history[i] = rs->history[i+1];
            // printf("rs->history[%d] IS = %s\n", i, rs->history[i]);
        }
        // The last command points to temp (which pointed to the old least recent cmdl (pos 0)
        // printf("rs->history[%d] WAS = %s\n", MAX_HISTORY-1, rs->history[MAX_HISTORY-1]);
        rs->history[MAX_HISTORY-1] = temp;
        // printf("rs->history[%d] IS = %s\n", MAX_HISTORY-1, rs->history[MAX_HISTORY-1]);

        // And we overwrite the last element and make it the new last most recent cmdl (pos 19)
        strcpy(rs->history[MAX_HISTORY-1], cmd);
        // printf("just added command_line: %s\n", rs->history[MAX_HISTORY-1]);
    }
    else {
        strcpy(rs->history[(rs->curr_pos)++], cmd);
        // printf("just added command_line: %s\n", rs->history[(rs->curr_pos)-1]);
    }
    // printf("just added command_line: %s\n", cmd);

    // All these need to be here so that the parsing of the asked command can be done in the right order afterwards
    temp = malloc(MAX_CMD_LENGTH * sizeof(char));
    strcpy(temp, cmd);
    cmd_args[0] = strtok(temp, " ");
    for (int i=1 ; (cmd_args[i-1] != NULL) ; i++) {
        cmd_args[i] = strtok(NULL, " ");
    }
    int f = strcmp(cmd_args[0], "myHistory")==0;
    if (f != 0) {
        // If there is NO specific command request
        if (strcmp(cmd_args[1], "\0") == 0) {
            print_history(rs, -1);
        }
        else {
        // If there is a specific command request
            int temp = atoi(cmd_args[1]);
            if (isFull(rs)) strcpy(cmd, rs->history[temp-2]);
            else strcpy(cmd, rs->history[temp-1]);
        }
    }
    else {
        // print_history(rs, -1);
    }
}