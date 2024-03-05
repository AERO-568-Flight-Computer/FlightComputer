#include <stdio.h>  // for printf
#include <stdlib.h>  // for exit
#include <string.h>  // for memset
#include <sys/socket.h>  // for socket
#include <netinet/in.h>  // for sockaddr_in
#include <arpa/inet.h>  // for inet_addr
#include <unistd.h>  // for close
#include "udp_send_float.h"

// Added more comments
// This function sends a float to the server
void udp_send_float(float number, int port) {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // if the socket creation fails, print an error message and exit
    if (sockfd < 0) {
        perror("Cannot create socket");
        exit(1);
    }

    // Set up the server address and port
    struct sockaddr_in serveraddr;
    // Zero out the server address
    memset(&serveraddr, 0, sizeof(serveraddr));
    // Set the server address family, port, and IP address
    serveraddr.sin_family = AF_INET;
    // Convert the port number to network byte order
    serveraddr.sin_port = htons(port);
    // Set the IP address of the server
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send the float number, if the sendto fails, print an error message and exit
    // Below, n is the return value of the sendto function. If n is less than 0, an error occurred
    int n = sendto(sockfd, &number, sizeof(number), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (n < 0) {
        perror("Error in sendto");
        exit(1);
    }

    close(sockfd);
}