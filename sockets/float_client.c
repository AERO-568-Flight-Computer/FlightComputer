#include <stdio.h>  // for printf and scanf
#include "udp_send_float.h"

int main() {
    float number;
    int port = 12345;  // Set the port here

    printf("Enter a float value between -90.0 and 90.0: ");
    scanf("%f", &number);

    // This calls the function I imported above from udp_send_float.h
    // The .h file is a header file that contains the function prototype
    // The udp_send_float.c file is the implementation file that 
    //    contains the function definition
    // Now, just call the function with the number and port
    // to send the float to the server
    udp_send_float(number, port);

    return 0;
}