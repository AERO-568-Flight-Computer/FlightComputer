#this is just an example of how you to implment a sockets server

import socket

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based server
server.bind(('localhost', 54321)) #puts the socket on a local port

server.listen() #tells the server to wait for a client to connect
client, address = server.accept() #waits for it to connect and creates objects for client and address
data = client.recv(1024) #tells server to expect a certian size packet and to assign that to data
print(data.decode('utf8')) #prints the decoded data
client.close() #closes the socket for the client, keeps it open for the server so someone else could connect