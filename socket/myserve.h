#ifndef __MYSERVE_H__
#define __MYSERVE_H__

#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>



//#include "common.h"


typedef struct 
{
    int client_fd;
    struct sockaddr_in s_addr;

}clientmes;


void sockettest();
int socket_create(int domain, int type, int protocol);
clientmes socket_accept(int server_fd);








#endif
