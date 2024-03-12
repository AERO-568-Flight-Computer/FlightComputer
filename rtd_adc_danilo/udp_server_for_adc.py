import socket
import time
import struct
import itertools
import numpy as np
from matplotlib import pyplot as plt

# udp_server_for_adc.py

# Create a UDP socket object
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the localhost and port number 12345
# This server only listens to the port 12345
server.bind(('localhost', 12347))

num_channels = 8

numIters = 0
# Loop forever and receive data from the client
while True:

    longestChannel = 0
    counter = 0


    for i in range(num_channels):
        # Receive data from the client with a maximum data buffer size of 4096 bytes (if data is getting truncated, check here)
        data, addr = server.recvfrom(1 << 12)

        num_ints_in_data = len(data) // 4


        # Each time we get back to the first channel, we want to append a new empty array to the data_from_adc array
        if i % num_channels == 0:
            try:
                np.append(data_from_adc, np.empty((num_ints_in_data,num_channels), np.float64) * np.nan, axis=1)
            except NameError:
                data_from_adc = np.empty((num_ints_in_data,num_channels), np.float64) * np.nan
            

        if num_ints_in_data > longestChannel:
            longestChannel = num_ints_in_data
            counter += 1
        elif num_ints_in_data < longestChannel:
            counter += 1

        # Unpack the data from the client into a tuple of integers
        unpacked_data = struct.unpack(str(num_ints_in_data) + 'i', data)
        # Place data into array at the correct channel
        data_from_adc[-num_ints_in_data:, i] = unpacked_data

    numIters += 1

    print(numIters)
    print(data_from_adc[-num_ints_in_data:, :])
    if counter > 1:
        print("Hmmmm")

    if numIters == 10:
        break
# Changes to make
# Initialize empty array that is 8 by num_ints_in_data
# apend empty array of that size on each loop iteration except the first at the start of the loop (I did think this through)
# Put the data into the array instead of appending

# Plot channel 1

plt.scatter(data_from_adc[:, 0])
plt.show()



# # Close the server socket when Ctrl+C is pressed
# server.close()
# # Print data_from_adc where each channel is a column
# for values in itertools.zip_longest(*data_from_adc, fillvalue='N/A'):
#     print(" ".join(map(str, values)))
# print(len(data_from_adc))
# print("wtf")
# print(data_from_adc)



