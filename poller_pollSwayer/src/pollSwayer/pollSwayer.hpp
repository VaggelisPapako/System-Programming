#pragma once
#include "../../lib/Utils.hpp"
#include <fstream>
#include <sstream>
#include <queue>
#include <cassert>

constexpr bool debug_message = false;

namespace PollSwayerConfig {
    constexpr int num_of_args = 3 + 1;

    struct poll_swayer_config_t {
        //NOTE: port should be made unique before 
        sockaddr_in server;
        int port;
        std::string input_file;

        poll_swayer_config_t(
            sockaddr_in _server,
            int _port,
            const char * _input_file
        )
        :server(_server),port(_port),input_file(_input_file)
        {}

        friend std::ostream& operator<<(std::ostream& os, const poll_swayer_config_t& conf)
        {
            os << "{" << inet_ntoa(conf.server.sin_addr) << ":" << conf.server.sin_port << ":" << conf.input_file << "}";
            return os;
        }
    };


    //returns nullptr on failure
    poll_swayer_config_t* parse_arguments(int argc, char* argv[]){
        if( argc != num_of_args )
            return nullptr;

        int port = strtol(argv[2], NULL, 10);
        
        if(!port)
            return nullptr;

        hostent* host = gethostbyname(argv[1]);
        sockaddr_in server;

        server.sin_family = AF_INET;
        memcpy ( &(server.sin_addr), host->h_addr,host->h_length );
        server.sin_port = htons(port);

        
        return new poll_swayer_config_t(server, port, argv[3]);

    }

    //returns <0 on error
    int parse_input_file(std::string& filename, std::queue< std::pair<std::string,std::string> >& q){
        std::ifstream input_file(filename);
        for( std::string line; std::getline(input_file, line); ){
            std::stringstream line_stream(line);
            std::string token;

            unsigned int count = 0;
            std::string name;
            std::string surname;
            std::string party;
            while (std::getline(line_stream, token, ' ')) {
                count++;


                if(count == 1){
                    name = token;
                }
                else if(count == 2){
                    surname = token;
                }
                else if(count == 3){
                    party = token;
                }
                else{
                    input_file.close();
                    return -1;
                }

            }
            if(debug_message){
                std::cout << "parsed: " << name << " " << surname << " " << party << std::endl;
            }
            q.push( std::make_pair(name+" "+surname, party) );
        }
        input_file.close();
        return 0;
    }
    
    //returns a socket that is ready to start sending data to the server
    int setup_connection_to_server(poll_swayer_config_t& config){
        int connection_sock;

        if ( ( connection_sock = socket(AF_INET,SOCK_STREAM,0) ) < 0 ){
            return -1;
        }

        //we want to be able to re-use ports
        const int enable = 1;
        if( setsockopt(connection_sock,SOL_SOCKET,SO_REUSEADDR, &enable,sizeof(int)) < 0){
            return -1;
        }

        if( ( connect(connection_sock, (sockaddr *) &config.server, sizeof(config.server)) ) < 0 ){
            return -1;
        }

        return connection_sock;
    }

}

struct worker_thread_info{
    std::queue< std::pair<std::string,std::string> >  votes_to_be_submited;
    pthread_mutex_t queue_mutex;

    PollSwayerConfig::poll_swayer_config_t* config;

    worker_thread_info(PollSwayerConfig::poll_swayer_config_t* _config)
    :config(_config)
    {
        //NULL means use default configuration
        pthread_mutex_init(&queue_mutex,NULL);
    }

    ~worker_thread_info(){
        pthread_mutex_destroy(&queue_mutex);
    }
};