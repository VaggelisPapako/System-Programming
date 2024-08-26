Evangelos-Papakostas-project2-sdi1800152

Compilation and execution:
Running make will produce both poller and pollSwayer binaries inside the build/ directory

Notable design decisions:
We make use of STL containers to cover our data-structure needs

Synchronization notes:
poller/pollSwayer: 
Worker threads synchronize by using a queue for that contains the ids of the sockets to-be-served.

poller: 
We use self-describing condition variables so that the master and worker threads can syncrhonize assuming the roles of a "producer" and "consumers" respectively. 
All condition variables correspond to the queue that contains the sockets ids to be served.

pollweSwayer: 
A worker thread may send more than 1 requests. This was done in order to have a symetric threading implementation with the server, 
where the threads syncrhonize over a queue.

Signal handling notes (SIGINT):
poller: 
Worker threads do not handle signals at all. The server will respond to dispatched signals in "ticks"; When a request is being handled, 
signal handling is turned off via sigprocmask. The program will check for the delivery of signals at the beginning of the program's loop. 
The program will also handle signals if it is delivered while blocking on an accept sys-call. 
To achieve this we make it so that sys-calls to don attempt to re-execute the interrupted instruction when a signal is delivered. 
This is achieved by clearing the SA_RESTART flag.

We do not receive signals directly by the os, but we rather delegate the delivery of signals to a file stream. 
This way, if a signal has been dispatched to the server process, read will successfully retrieve the information regarding it. 
Otherwise, the read would have blocked but we wish to open the signal-dispatching file in non-blocking mode.

Our implementation allows us to process the event of a signal delivery in a completely differed manner. 
We do not need to handle the signal in any way, we simply take notice of the delivery and gracefully exit the main loop. 
To override the default signal handling behavior ( which would terminate the server process ), we specify a no-op function as the signal handler.