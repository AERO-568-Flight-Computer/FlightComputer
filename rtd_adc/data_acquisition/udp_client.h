// Written by Danilo to allow a 3 stage send process where the server is opened  and closed independently of the send process

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <stdio.h>  // for printf
#include <stdlib.h>  // for exit
#include <string.h>  // for memset
#include <sys/socket.h>  // for socket
#include <netinet/in.h>  // for sockaddr_in
#include <arpa/inet.h>  // for inet_addr
#include <unistd.h>  // for close
// #include "udp_send_data.h" // might need to add a header back in, not sure
#include <errno.h> 

struct udp_client {
    int sockfd;
    struct sockaddr_in serveraddr;
};

void udp_start_client(struct udp_client *client, int port);

void udp_send_data_v2(struct udp_client *client, void* data, size_t size);

void udp_end_client(struct udp_client *client);

#endif // UDP_CLIENT_H