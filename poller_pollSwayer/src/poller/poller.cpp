#include "poller.hpp"

//returns <0 or error
int serve_connection(worker_thread_info* tinfo, int socket){
    Serializer serializer;
    int success;
    serializer.bind(socket);

    if( serializer.send_string("SEND NAME PLEASE") < 0 ){
        std::cerr << "[" << pthread_self() << "]" << "{SEND NAME PLEASE} could not be sent\n";
    }

    std::string voter;
    success = serializer.receive_string(voter);
    assert(success != -1);
    
    pthread_mutex_lock( &(tinfo->stats_mutex ) );
    if(!tinfo->server_stats.can_vote(voter)){
        success = serializer.send_string("ALREADY VOTED");
        assert(success != -1);
        
        pthread_mutex_unlock( &(tinfo->stats_mutex ) );
        close(socket);
        return 0;
    }
    pthread_mutex_unlock( &(tinfo->stats_mutex ) );

    if( serializer.send_string("SEND VOTE PLEASE") < 0 ){
        std::cerr << "[" << pthread_self() << "]" << "{SEND VOTE PLEASE} could not be sent\n";
    }
    std::string party;
    success = serializer.receive_string(party);
    assert(success != -1);
    close(socket);

    pthread_mutex_lock( &(tinfo->stats_mutex ) );
    success = tinfo->server_stats.insert_vote(voter, party);
    if(!success){
        std::cerr << "voter: " << voter << " has already voted supposedly" << "(tried to vote for "<<  party <<" )\n";
        /*
            ALREADY VOTED cannot be sent right now, you will have already closed the connection.

            At this point, it's a matter of keepint the log data in a consistent state without crashing the app
        */
        
        pthread_mutex_unlock( &(tinfo->stats_mutex ) );
        close(socket);
        return 0;
    }

    pthread_mutex_unlock( &(tinfo->stats_mutex ) );
    
    pthread_mutex_lock( &(tinfo->log_file_mutex) );

    std::ofstream log_file_out(tinfo->config->poll_log,std::ios_base::app); //open in append mode
    log_file_out << voter << " " << party << "\n";
    log_file_out.flush();
    log_file_out.close();

    pthread_mutex_unlock( &(tinfo->log_file_mutex) );
    return success;
}

void* worker_thread(void * arg){
    worker_thread_info* tinfo = (worker_thread_info *) arg;

    while(1){
        pthread_mutex_lock( &(tinfo->queue_mutex) );
        while( !tinfo->sockets_to_serve.size() ){
            pthread_cond_wait( &(tinfo->filled_buffer_cond), &(tinfo->queue_mutex) );
        }

        int socket_to_serve = tinfo->sockets_to_serve.front();
        tinfo->sockets_to_serve.pop();

        pthread_cond_broadcast(&(tinfo->free_space_in_buffer_cond));
        pthread_mutex_unlock( &(tinfo->queue_mutex) );

        serve_connection(tinfo, socket_to_serve);
    }
}

//returns true if SIGINT has been received
bool pending_termination(int sfd){
    signalfd_siginfo fdsi;

    int s = read(sfd, &fdsi, sizeof(fdsi));
    if(s == EAGAIN)
        return false;
    
    return fdsi.ssi_signo == SIGINT;
}

void noop_signal_handler(int){
    return;
}


int main(int argc, char* argv[]) {
    PollerConfig::poller_config_t* config{ PollerConfig::parse_arguments(argc,argv) };

    if(!config){
        std::cerr << "Argument parsing failure\n";
        return 1;
    }
    std::cout << "Server Config: " << *config << std::endl;

    int listener_sock = PollerConfig::setup_listener(*config);
    if(listener_sock < 0){
        std::cerr << "The listener socket could not be setup\n";
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = noop_signal_handler;
    sa.sa_flags &= ~SA_RESTART; // clear SA_RESTART flag so that accept returns when cntrl+c is pressed
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        std::cerr << "The listener socket could not be setup\n";
        return 1;
    }
    
    /*
        handle signals by reading a file! : https://man7.org/linux/man-pages/man2/signalfd.2.html
        Singal handling will be deferred to points in which the main loop can gracefully terminate the process.

        Dealing  with async signal handlers, multiple threads and potential memory leaks would otherwise be too much :)
    */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    if ( pthread_sigmask( SIG_BLOCK, &mask, 0 ) < 0){
        std::cerr << "Could not setup sigmask\n";
        return 1;
    }
    int sfd = signalfd(-1, &mask, SFD_NONBLOCK);
    if(sfd < 0){
        std::cerr << "Could not setup signal-polling file\n";
        return 1;
    }


    worker_thread_info* tinfo = new worker_thread_info(config);
    pthread_t* tid = new pthread_t[config->n_workers];
    for(unsigned int i = 0; i < config->n_workers; i++){
        pthread_create(&(tid[i]),NULL,worker_thread,(void *) tinfo);
    }

    std::cout << "Listening on port: " << ntohs(config->server.sin_port) << std::endl;
    while(1){
        if(pending_termination(sfd))
            break;
        
        int new_sock;
        //The only acceptable point to be interupted by a signal
        if ( pthread_sigmask( SIG_UNBLOCK, &mask, 0 ) < 0){
            std::cerr << "Could not unblock signal\n";
            return 1;
        }  

        if( (new_sock = accept(listener_sock,NULL,NULL) ) < 0){
            if(errno != EINTR /*Interupted by signal*/){
                std::cerr << "Could not accept incomming connection" << new_sock << "\n";
                perror("accept: ");
                return 1;
            }
            break;
        }

        //continue blocking SIGINT
        if ( pthread_sigmask( SIG_BLOCK, &mask, 0 ) < 0){
            std::cerr << "Could not re-block signal\n";
            return 1;
        }


        pthread_mutex_lock( &(tinfo->queue_mutex) );
        while( tinfo->sockets_to_serve.size() == config->n_connections ){
            /*
                https://linux.die.net/man/3/pthread_cond_wait

                Spurious wakeups from the pthread_cond_timedwait() or pthread_cond_wait() functions may occur. 
                Since the return from pthread_cond_timedwait() or pthread_cond_wait() does not imply anything about the value of this predicate, 
                the predicate should be re-evaluated upon such return.
            */
            pthread_cond_wait( &(tinfo->free_space_in_buffer_cond), &(tinfo->queue_mutex) );
        }

        tinfo->sockets_to_serve.push(new_sock);
        //Before pthread_cond_returns there will be an attempt to re-acquire the lock
        pthread_cond_broadcast(&(tinfo->filled_buffer_cond));
        pthread_mutex_unlock( &(tinfo->queue_mutex) );

    }
    tinfo->server_stats.print_party_stats(config->poll_stats);
    std::cout << "Cleaning up ..." <<std::endl;
    for(unsigned int i=0;i < config->n_workers;i++){
        pthread_cancel(tid[i]);
        pthread_detach(tid[i]);
    }

    close(listener_sock);
    close(sfd);
    delete tinfo;
    delete config;
    delete[] tid;

    return 0;
}