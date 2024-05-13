
#include "udp_client.h"

int udp_main_guide() {

    // Create a client
    struct udp_client client;

    // Initialize the client
    udp_start_client(&client, 12345);

    // Zero out server address
    memset(&client.serveraddr, 0, sizeof(client.serveraddr));

    // Use the client...
    // For example, to send data:
    sendto(client.sockfd, data, size, 0, (struct sockaddr*)&client.serveraddr, sizeof(client.serveraddr));

    // Close the client
    udp_end_client(&client);

    return 0;

}