This folder contains the code for reading data from the analog to digital converter (rtd DM35424)

The "driver_info" folder contains the README for the driver from the manufacturer and other info. That README explains how to load the driver and which folders need to be compiled to run the code

The "data_acquisition" folder contains the modified example code that sends data from the ADC to a python server.

Please run udp_server_for_adc.py to start the python server. This server stores the data from the adc in an array. It also plots the data from channel 1. Right now, it only runs for 10 cycles of data collection. This can easily be changed

Run ./dm35424_adc_for_cpopa.c --binary to send data to the python server. The proces currently is too slow and doesn't keep up, but that will be changed soon. I am working on a set of three functions in C that establish the client, send data, and close the client respectively, which will significantly increase the speed.

Those functions are:
udp_start_client.c
udp_send_data_v2.c
udp_end_client.c

With header:
udp_client.h

These might be working but I haven't tested yet. They need to be implemented into the main c file.