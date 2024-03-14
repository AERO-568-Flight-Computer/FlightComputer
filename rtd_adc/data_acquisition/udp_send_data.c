#include <stdio.h>  // for printf
#include <stdlib.h>  // for exit
#include <string.h>  // for memset
#include <sys/socket.h>  // for socket
#include <netinet/in.h>  // for sockaddr_in
#include <arpa/inet.h>  // for inet_addr
#include <unistd.h>  // for close
#include "udp_send_data.h"
#include <errno.h>  

// make sure to include the header file udp_send_data.h in the code that you want to call this function in
// Check later if the port number has some way of checking that it is not in use. If we're doing a lot of this, we'll need to 
// have a way of making sure we're not communicating over the same port. Actually,
// What we might end up doing is having the server communicate with every subprocess over one port,
// so once the server is established, the port will be reserved, and it will need to communicate to each process which is
// correct port to talk over.

/*
Future

- Check with Kurt if we can avoid reopening the port every time we call this function. 
  That is probaly a slow operation. We could keep the port open and just send data over it.
*/


// Added more comments
// This function sends any data to the server
void udp_send_data(void* data, size_t size, int port) {
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


    // Send the data, if the sendto fails, print an error message and exit
    // Below, n is the return value of the sendto function. If n is less than 0, an error occurred
    int n = sendto(sockfd, data, size, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (n < 0) {
        perror("Error in sendto");
        exit(1);
    }

    close(sockfd);
}
