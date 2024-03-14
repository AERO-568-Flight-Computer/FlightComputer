
#include "udp_client.h"

// This function starts a UDP client that can be used to send data to a server
// Make sure to run the udp_end_client function when you are done with the client

void udp_start_client(struct udp_client *client, int port) {
    // Create a socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // if the socket creation fails, print an error message and exit
    if (client->sockfd < 0) {
        perror("Cannot create socket");
        exit(1);
    }

    // // Set up the server address and port
    // struct sockaddr_in serveraddr;
    // // Zero out the server address
    // memset(&serveraddr, 0, sizeof(serveraddr));
    // Set the server address family, port, and IP address
    client->serveraddr.sin_family = AF_INET;
    // Convert the port number to network byte order
    client->serveraddr.sin_port = htons(port);
    // Set the IP address of the server
    client->serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


}