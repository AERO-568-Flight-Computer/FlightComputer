#this is just an example of how you to implment a sockets client to talk to serverTest.py

import socket
import time

time.sleep(0.5) #adds delay to make sure that the server is setup

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based server
client.connect(('localhost', 54321)) #connects socket to a local port
client.send(b'hello') #sends a message, encoded in binary