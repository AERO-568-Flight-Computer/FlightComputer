# udp_server.py
import socket
import time
import struct

# Create a UDP socket object
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the localhost and port number 12345
# This server only listens to the port 12345
server.bind(('localhost', 12345))

# Loop forever and receive data from the client
while True:
    # Receive data from the client with a maximum data buffer size
    data, addr = server.recvfrom(1024)
    # The data received is in bytes, so convert it to a float
    move_to = struct.unpack('f', data)
    print('From address:', addr)
    print(move_to[0])
