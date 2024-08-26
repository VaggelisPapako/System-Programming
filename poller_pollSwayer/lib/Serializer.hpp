#pragma once
#include <unistd.h>
#include <string>
#include <cstring>

#include <iostream>
#include <errno.h>

/*
    Library to serialize strings over sockets using the `read/write` systemcalls

    NOTE: one could easily write a generalized serializer using templates, however this exercise ( K24 ) requires only strings to be serialized 
*/
class Serializer{
    static constexpr unsigned int default_size = 1024;

    private:
        //IMPORTANT: m_buffer_size will be initialized 1st if it is declared 1st
        int m_socket;

        int _send_bytes(const char* str, unsigned int length){
            unsigned int offset = 0;

            do{
                int n_transferred_bytes = write(m_socket, &( str[offset] ), length);
                if(n_transferred_bytes < 0)
                    return -1;
                
                offset += n_transferred_bytes;
                length -= n_transferred_bytes;
            }while(length);

            return 0;
        }

        int _receive_bytes(char* str, unsigned int length){
            unsigned int offset = 0;
            
            do{
                fd_set fset;
                FD_ZERO(&fset);
                FD_SET(m_socket,&fset);
                select(m_socket+1,&fset,NULL,NULL,NULL);

                int n_transferred_bytes = read(m_socket, &( str[offset] ), length);
                if(n_transferred_bytes < 0){
                    perror("_receive_bytes::read :");
                    return -1;
                }
                
                // if(!n_transferred_bytes)
                //     return -1;
                
                length -= n_transferred_bytes;
                offset += n_transferred_bytes;
            }while(length);

            return 0;   
        }

    public:
        Serializer(int socket)
        :m_socket( socket )
        {}

        Serializer()
        {}

        int bind(int _socket){
            return (m_socket = _socket);
        }

        /*
            Serializes a string over a socket in Java fashion:
            - length is sent first
            - string characters are then sent

            returns 0 on success and < 0 on error
        */
        int send_string(const char * str, unsigned int length){
            //treat the integer as a generic byte-buffer when serializing
            int err = _send_bytes((char* ) &length, sizeof(unsigned int));
            if(err < 0){
                return -1;
            }
            
            return _send_bytes(str,length);
        }

        int send_string(const std::string& str){
            return send_string(str.c_str(),str.length());
        }

        //upon successful serialization the function allocates space in the heap with `new`
        int receive_string(char*& str){
            unsigned int length;
            //treat the integer as a generic byte-buffer when serializing
            int err = _receive_bytes((char* ) &length, sizeof(unsigned int));
            if(err < 0)
                return -1;
            
            str = new char[length+1];
            //will not receive more than length bytes -- avoid buffer overflows
            err = _receive_bytes(str,length);
            str[length] = '\0';
            if(err < 0){
                delete[] str;
            }

            return err;
        }

        int receive_string(std::string& str){
            char* raw_string;
            if( receive_string(raw_string) < 0 )
                return -1;
            
            str = std::string(raw_string);
            delete[] raw_string;
            return 0;
        }
};