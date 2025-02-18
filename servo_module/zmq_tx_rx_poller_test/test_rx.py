#Poller example.
#I am goin to make a couple sockets in a different file.
#Send stuff, with socketsd sending different messages at different times.
#Then I will make a poller, and see if it can receive all the messages.
#And print them out.

import zmq
import struct
context = zmq.Context()

rx_socket1 = context.socket(zmq.PULL)
rx_socket1.connect('tcp://localhost:5556')

rx_socket2 = context.socket(zmq.PULL)
rx_socket2.connect('tcp://localhost:5557')

# Without poller
""" while True:
    message1 = rx_socket1.recv()
    print(f"Received message1: {struct.unpack('4sf',message1)}")

    message2 = rx_socket2.recv()
    print(f"Received message2: {struct.unpack('4sf',message2)}") """

# With poller
poller = zmq.Poller()
poller.register(rx_socket1, zmq.POLLIN)
poller.register(rx_socket2, zmq.POLLIN)

#The tutorial says socks = dict(sock.poll())

while True:
    socks = dict(poller.poll())
    if rx_socket1 in socks and socks[rx_socket1] == zmq.POLLIN:
        message1 = rx_socket1.recv()
        print(f"Received message1: {struct.unpack('4sd',message1)}")
        print(socks)

    if rx_socket2 in socks and socks[rx_socket2] == zmq.POLLIN:
        message2 = rx_socket2.recv()
        print(f"Received message2: {struct.unpack('4sd',message2)}")
        print(socks)