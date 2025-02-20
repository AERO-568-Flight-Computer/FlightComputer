# udp_client.py
import socket
import time
import struct

# Create a socket object using UDP (not TCP)
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Now I can just send to whatever is listening on port 12345 on the localhost
while True:
    message = float(input('Enter the new position for the servo in degrees from -90.0 to 90.0: '))

    # Convert the float to bytes, as we can only send bytes
    message_bytes = struct.pack('f', message)
    print('Sending:', message, message_bytes)
    client.sendto(message_bytes, ('localhost', 12300))
    time.sleep(1)  
