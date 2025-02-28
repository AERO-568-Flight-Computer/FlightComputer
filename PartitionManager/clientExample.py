#example code of what the partiton manager expects from a partiton to know it has been initalized

from partitonManager import initialize

initialize.initialize()

# import socket
# import time

# time.sleep(0.5) #adds delay to make sure that the server is setup
# client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based socket
# client.connect(('localhost', 54321)) #connects socket to the partiton manager as a client
# client.send(b'success') #sends a message that tells the partiton manager that initialization has been completed