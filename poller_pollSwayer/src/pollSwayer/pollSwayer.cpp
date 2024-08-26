#include "pollSwayer.hpp"

void* worker_thread(void * arg){
    worker_thread_info* tinfo = (worker_thread_info*) arg;

    while(1){

        pthread_mutex_lock(&(tinfo->queue_mutex));
        if(tinfo->votes_to_be_submited.empty()){
            pthread_mutex_unlock(&(tinfo->queue_mutex));
            break;
        }
        auto pair = tinfo->votes_to_be_submited.front();
        tinfo->votes_to_be_submited.pop();
        pthread_mutex_unlock(&(tinfo->queue_mutex));

        int connection_sock = PollSwayerConfig::setup_connection_to_server( *(tinfo->config) );
        assert(connection_sock != -1);

        Serializer serializer{connection_sock};
        std::string msg;

        int success = serializer.receive_string(msg);
        assert(success != -1);
        if(msg != "SEND NAME PLEASE"){
            std::cerr << "[" << pthread_self() << "]" << "corrupted server message detected: " << msg << "\n";
        }
        if(serializer.send_string(pair.first) < 0){
            std::cerr << "[" << pthread_self() << "]" << "could not send name: " << pair.first << "\n";
        }

        success = serializer.receive_string(msg);
        assert(success != -1);
        if( (msg != "ALREADY VOTED") && (msg != "SEND VOTE PLEASE") ){
            std::cerr << "[" << pthread_self() << "]" << "corrupted server message detected: " << msg << "\n";
        }

        if(msg != "ALREADY VOTED"){
            serializer.send_string(pair.second);
        }
        else{
            std::cerr << "ALREADY VOTED for entry {" << pair.first << " , " << pair.second << "}\n";
        }

        if(debug_message){
            std::cout << "Send pair {" << pair.first << " , " << pair.second << "}" << std::endl;
        }

        close(connection_sock);
        
    }
    
    pthread_exit((void *)0);
}

int main(int argc, char* argv[]) {
    PollSwayerConfig::poll_swayer_config_t* config{ PollSwayerConfig::parse_arguments(argc,argv) };

    if(!config){
        std::cerr << "Argument parsing failure\n";
        return 1;
    }
    std::cout << "Client Config: " << *config << std::endl;
    worker_thread_info* tinfo = new worker_thread_info(config);

    if( PollSwayerConfig::parse_input_file(config->input_file,tinfo->votes_to_be_submited) < 0){
        std::cerr << "inputFile parsing failure\n";
        return 1;
    }
    
    const unsigned int n_worker_threads = tinfo->votes_to_be_submited.size(); 
    pthread_t* tid = new pthread_t[n_worker_threads];
    for(unsigned int i = 0; i < n_worker_threads; i++){
        pthread_create(&tid[i],NULL,worker_thread,(void *) tinfo);
    }

    void* return_val;
    for(unsigned int i=0;i < n_worker_threads;i++){
        pthread_join(tid[i],&return_val);
        if(return_val == 0)
            std::cout << "thread sucessfully terminated\n";
        else
            std::cout << "reason: " << return_val <<"\n";
    }

    delete config;
    delete tinfo;
    delete[] tid;
    return 0;
}