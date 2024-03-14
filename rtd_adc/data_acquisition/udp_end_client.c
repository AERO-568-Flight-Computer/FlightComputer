#include "udp_client.h"


// This function closes an existing udp client
void udp_end_client(struct udp_client *client) {
    // Close the socket
    close(client->sockfd);
}

