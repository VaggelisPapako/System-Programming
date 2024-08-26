#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    char** history;     // Most recent command is the one history[19] (or hte one closest to it) // gia na mporw na allazw tous pointers
    int curr_pos;       // shows the next empty spot on History array | ==0 means Q is empty | == 20 means Q is full
                        // its actually used only for the first filling of the array, then the array stays full
} reverse_stack;

void init_rs(reverse_stack* rs);
void clean_rs(reverse_stack* rs);
int isFull(reverse_stack* rs);
int isEmpty(reverse_stack* rs);
void print_history(reverse_stack* rs, int num);
void add_to_history(reverse_stack* rs, char* cmd);

#endif