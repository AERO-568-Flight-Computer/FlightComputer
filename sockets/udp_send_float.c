#include <stdio.h>  // for printf
#include <stdlib.h>  // for exit
#include <string.h>  // for memset
#include <sys/socket.h>  // for socket
#include <netinet/in.h>  // for sockaddr_in
#include <arpa/inet.h>  // for inet_addr
#include <unistd.h>  // for close
#include "udp_send_float.h"

void udp_send_float(float number, int port) {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Cannot create socket");
        exit(1);
    }

    // Set up the server address and port
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send the float number
    int n = sendto(sockfd, &number, sizeof(number), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (n < 0) {
        perror("Error in sendto");
        exit(1);
    }

    close(sockfd);
}