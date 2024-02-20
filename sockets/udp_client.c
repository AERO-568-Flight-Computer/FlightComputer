#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    float send_data;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket");
        return 0;
    }

    // Set up server address
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(SERVER_PORT);

    // Prompt user to enter a value between -90 and 90
    do {
        printf("Enter a value between -90 and 90: ");
        scanf("%f", &send_data);
    } while (send_data < -90.0 || send_data > 90.0);

    // Send the float to the server
    if (sendto(sockfd, &send_data, sizeof(send_data), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("sendto failed");
        return 0;
    }

    printf("Sent the float to the server\n");

    close(sockfd);
    return 0;
}