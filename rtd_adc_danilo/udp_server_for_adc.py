import socket
import time
import struct
import itertools
import numpy as np

# udp_server_for_adc.py

# Create a UDP socket object
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the localhost and port number 12345
# This server only listens to the port 12345
server.bind(('localhost', 12347))

num_channels = 8

# Loop forever and receive data from the client
while True:

    data_from_adc = np.empty((num_channels, 0), )
    longestChannel = 0
    counter = 0

    for i in range(num_channels):
        # Receive data from the client with a maximum data buffer size of 4096 bytes (if data is getting truncated, check here)
        data, addr = server.recvfrom(1 << 12)

        num_ints_in_data = str(len(data) // 4)

        if int(num_ints_in_data) > longestChannel:
            longestChannel = int(num_ints_in_data)
            counter += 1
        elif int(num_ints_in_data) < longestChannel:
            counter += 1

        unpacked_data = struct.unpack(num_ints_in_data + 'i', data)
        column_to_append = np.array(unpacked_data).reshape(-1, 1)
        data_from_adc = np.append(data_from_adc, column_to_append, axis=0)
    for values in zip(*data_from_adc):
        print("\t".join(map(str, values)))

    if counter > 1:
        print("Hmmmm")

# Changes to make
# Initialize empty array that is 8 by num_ints_in_data
# apend empty array of that size on each loop iteration except the first at the start of the loop (I did think this through)
# Put the data into the array instead of appending
    



# # Close the server socket when Ctrl+C is pressed
# server.close()
# # Print data_from_adc where each channel is a column
# for values in itertools.zip_longest(*data_from_adc, fillvalue='N/A'):
#     print(" ".join(map(str, values)))
# print(len(data_from_adc))
# print("wtf")
# print(data_from_adc)



