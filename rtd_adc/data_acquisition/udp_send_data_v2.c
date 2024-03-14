#include "udp_client.h"

// This function send data via an existing UDP client
// You must initialize the client with udp_start_client before using this function

void udp_send_data_v2(struct udp_client *client, void* data, size_t size) {
    // Send the data
    sendto(client->sockfd, data, size, 0, (struct sockaddr*)&client->serveraddr, sizeof(client->serveraddr));
}