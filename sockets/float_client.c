#include <stdio.h>  // for printf and scanf
#include "udp_send_float.h"

int main() {
    float number;
    int port = 12345;  // replace with your actual port number

    printf("Enter a float value between -90.0 and 90.0: ");
    scanf("%f", &number);

    udp_send_float(number, port);

    return 0;
}