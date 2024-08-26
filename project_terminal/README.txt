
############## SOOOOOOOOOOOOOOS NA SUMPLHRWSW TON PINAKA DOCX POU EXEI STO PIAZZA EINAI UPOXREWTIKOS STHN PARADOSH ###################

# Όσον αφορά τα pipes:
...
ελέγχουμε αν είναι το 1ο, ενδιάμεσο ή τελευταίο pipe ώστε να ξέρουμε πού να στείλουμε το κάθε input-output
...

na kanw elegxous gia to an petuxe h malloc

SOOOS xreiazetai na grapsw tosa exit ~ osa alias exw kanei execute nmzw? + meta petaei Seg - na to dw




douleuoun:
cat file1
diff file1 file2
ls
ls -l
ls -l -r
ls -l -r -a
ls sus*             OXI->wildchar handler needed
wc -l
wc -w
wc -c
tail -10 file
head -5 file

pipes:
genika ka8e command -> input -> fd[i-1][0]
                    -> output -> fd[i][1]
eite einai 1o eite endiameso eite teleutaio command
sto susspro1.c -> cat susspro1.c | grep "pid"
douleuei mono ws: cat susspro1.c | grep pid -> gt kanei apo mono tou to pid string  OK ======= SOOOOOOOOOOS na mpei README
ls -l | wc -l                                   OK
ls -l | grep vaggelispapako | wc -l             OK
diff suspro1.c susspro1.c | wc -l               OK
diff suspro1.c susspro1.c | grep pipes          OK
diff suspro1.c susspro1.c | grep pipe | wc -l   OK
diff functions.c main.c | grep pipe | wc -l     OK




alias:
I realloc their array (only grows), but i only chift them lost to destroy (extra scape stays useless)
mpainei parse_input() -> alias_flag=1 (if command) -> paei sto (else non-command) -> mpainei sto (if alias_flag) afou egine 1 ->
-> ...
Prepei na mporw na kalesw alias + argumentst extra meta px: createalias ll "ls -l" -> ll -r -a -> k na douleuei san: ls -l -r -a
prepei na allaxw arketa to pou kanw handle ta aliases gia na ginei auto-^ + thn apo8hkeush tous


echo hi | grep i
createalias ll "ls -l -r -a"                    OK
createalias lll "ls -l"                         OK
createalias lpl "cat suspro1.c"                 OK
createalias lll "ls -l | wc -l"                 OK
createalias qq "cat suspro1.c | wc -l"          OK (7/5 - 5.37pm)
createalias qqq "ls -l | grep vaggelis | wc -l" OK
#   createalias qq "cat suspro1.c"
    qq | wc -l                                  OK
#
#
    createalias ll "ls -l"
    ll -r -a                                    OXI
#

history:
myHistory me alias me pipes -> douleuei
my reverse_stack functionality:
ftiaxnw xwro gia 20 strings-command_lines
sthn arxh gemizw seiriaka tis 8eseis autes
kai otan gemisoun -> enas boh8htikos pointer deixnei sthn 1h 8esh (paliotero cmdl) ->
h 8esh 0 deixnei sthn 8esh 1, h 1 sthn 2, ... h proteleutaia sthn teleutaia ->
h teleutaia 8esh deixnei ston boh8htiko pointer (dld sthn palia 8esh 0) -> kai allazei ta periexomena ths me to NEO CMDL ->
epomenws h palia 8esh 0 -> ginetai h kainouria 8esh 19 (teleutaia 8esh = neotero cmdl)

### exei 8ema mono sthn oriakh periptwsh pou pathseis myHistory N, otan exoun gemisei <N 8eseis (trexei thn prohgoumenh entolh)
### an exoun gemisei >N trexei swsta thn epilogh pou fainetai

kanw print_history se ka8e entolh k oxi mono otan to zhtaei

otan grafe Exit -> den mpainei sto history (mporei k na prepei na mhn mpaine -> afou 8a bgoume k 8a xanarxisoume to shell)

uparxei akomh to 8ema pou ginetai print to history 2 fores (1 sthn suanrthsh k 1 sth main meta) (7/5)













#define MAX_SIZE 20

typedef struct {
    char commands[MAX_SIZE][100];
    int top;
} Stack;

// Function to initialize the stack
void initializeStack(Stack* stack) {
    stack->top = -1;
}

// Function to check if the stack is empty
int isEmpty(Stack* stack) {
    return (stack->top == -1);
}

// Function to check if the stack is full
int isFull(Stack* stack) {
    return (stack->top == MAX_SIZE - 1);
}

// Function to push an element onto the stack
void push(Stack* stack, const char* command) {
    if (isFull(stack)) {
        printf("Stack is full. Cannot push more elements.\n");
        return;
    }

    stack->top++;
    strcpy(stack->commands[stack->top], command);
}

// Function to pop an element from the stack
void pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty. Cannot pop element.\n");
        return;
    }

    stack->top--;
}

// Function to display the contents of the stack
void print_stack(Stack* stack) {
    printf("Command History:\n");
    for (int i = stack->top; i >= 0; i--) {
        printf("%s\n", stack->commands[i]);
    }
}

int main() {
    Stack history;
    initializeStack(&history);

    // Example usage
    push(&history, "ls");
    push(&history, "cd");
    push(&history, "mkdir");
    print_stack(&history);

    pop(&history);
    print_stack(&history);

    return 0;
}