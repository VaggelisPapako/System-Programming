
# Όσον αφορά τα pipes:
...
ελέγχουμε αν είναι το 1ο, ενδιάμεσο ή τελευταίο pipe ώστε να ξέρουμε πού να στείλουμε το κάθε input-output
...

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
