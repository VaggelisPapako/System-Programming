#pragma once
#include "../../lib/Utils.hpp"
#include <unordered_map>
#include <cassert>
#include <cerrno>
#include <queue>
#include <fstream>

#include <sys/signalfd.h>
#include <signal.h>

namespace PollerConfig {
    constexpr int num_of_args = 5 + 1;

    struct poller_config_t {
        sockaddr_in server;
        unsigned int n_workers;
        unsigned int n_connections;
        std::string poll_log;
        std::string poll_stats;

        poller_config_t(
            sockaddr_in& _server,
            unsigned int _n_workers,
            unsigned int _n_connections,
            const char * _poll_log,
            const char * _poll_stats
        )
        :server(_server),n_workers(_n_workers),n_connections(_n_connections),poll_log(_poll_log),poll_stats(_poll_stats)
        {}

        friend std::ostream& operator<<(std::ostream& os, const poller_config_t& conf)
        {
            os << "{" << ntohs(conf.server.sin_port) << ":" << conf.n_workers << ":" << conf.n_connections << ":" << conf.poll_log << ":" << conf.poll_stats << "}";
            return os;
        }
    };


    //returns nullptr on failure
    poller_config_t* parse_arguments(int argc, char* argv[]){
        if( argc != num_of_args )
            return nullptr;

        int port = strtol(argv[1], NULL, 10);
        unsigned int n_workers = strtoul(argv[2], NULL, 10);
        unsigned int n_connections = strtoul(argv[3], NULL, 10);
        
        if(!port || !n_workers || !n_connections)
            return nullptr;

        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(port);

        return new poller_config_t(server, n_workers, n_connections, argv[4], argv[5]);

    }
    
    //returns a socket that is ready to start accepting connections
    int setup_listener(poller_config_t& config){
        int listener_sock;

        if ( ( listener_sock = socket(AF_INET,SOCK_STREAM,0) ) < 0 ){
            return -1;
        }

        //we want to be able to re-use ports
        const int enable = 1;
        if( setsockopt(listener_sock,SOL_SOCKET,SO_REUSEADDR, &enable,sizeof(int)) < 0){
            return -1;
        }

        if( ( bind(listener_sock, (sockaddr *) &config.server, sizeof(config.server)) ) < 0 ){
            return -1;
        }

        //leaving some leeway on the number of backlog connections
        if( (listen(listener_sock,1000) )  < 0){
            return -1;
        }

        return listener_sock;
    }

}

class PollStats{
    std::unordered_map<std::string, std::string > m_voter_to_party;
    std::unordered_map<std::string, unsigned int> m_party_to_num_votes;
    public:

    //returns false if the vote has been counted already
    bool insert_vote(std::string& voter, std::string& party){
        // if the voter is in the map, he has voted before
        auto vote_exists = m_voter_to_party.find( voter );
        if(vote_exists != m_voter_to_party.end() )
            return false;

        auto insertion_pair = m_voter_to_party.insert( std::make_pair(voter,party) );
        //sanity check that insertion was successful
        assert(insertion_pair.second);

        auto party_exists = m_party_to_num_votes.find(party);
        if(party_exists == m_party_to_num_votes.end()){
            //1st vote for the party
            auto party_insertion = m_party_to_num_votes.insert( std::make_pair(party,1) );
            assert(party_insertion.second);
        }
        else{
            m_party_to_num_votes[party]++;
        }

        return true;
    }

    //returns true if the voter has not voted already
    bool can_vote(std::string& voter){
        // if the voter is in the map, he has voted before
        auto vote_exists = m_voter_to_party.find( voter );
        
        return vote_exists == m_voter_to_party.end();
    }


    void print_party_stats(std::string& fname){
        std::ofstream stat_file(fname);

        unsigned int sum = 0;
        for(auto i = m_party_to_num_votes.begin() ; i != m_party_to_num_votes.end() ;i++){
            stat_file << i->first << " " << i->second << "\n";
            sum += i->second;
        }
        stat_file << "TOTAL " << sum << "\n"; 

        stat_file.flush();
        stat_file.close();
    }

};

struct worker_thread_info{
    PollStats server_stats;
    pthread_mutex_t stats_mutex;

    std::queue<int> sockets_to_serve;
    pthread_mutex_t queue_mutex;

    pthread_cond_t free_space_in_buffer_cond;
    pthread_cond_t filled_buffer_cond;

    //we do not assume ownershipt of this pointer ( i.e. will not free it )
    PollerConfig::poller_config_t* config;

    pthread_mutex_t log_file_mutex;
    int log_file_fd;

    worker_thread_info(PollerConfig::poller_config_t* _config)
    :config(_config)
    {
        //NULL means use default configuration
        pthread_mutex_init(&stats_mutex,NULL);
        pthread_mutex_init(&queue_mutex,NULL);
        pthread_mutex_init(&log_file_mutex,NULL);


        pthread_cond_init(&free_space_in_buffer_cond,NULL);
        pthread_cond_init(&filled_buffer_cond,NULL);
    }

    ~worker_thread_info(){
        pthread_mutex_destroy(&stats_mutex);
        pthread_mutex_destroy(&queue_mutex);
        pthread_mutex_destroy(&log_file_mutex);


        pthread_cond_destroy(&free_space_in_buffer_cond);
        pthread_cond_destroy(&filled_buffer_cond);
    }
};