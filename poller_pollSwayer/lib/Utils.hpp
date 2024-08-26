#pragma once

#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /*gethostbyaddr*/
#include <unistd.h> /* fork */
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>


#include <ctype.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#include "./Serializer.hpp"