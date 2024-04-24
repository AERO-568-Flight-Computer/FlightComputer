import socket
from volz_actuator import build_pos_command
import serial
import struct

# Initialize serial connection to the actuator
ser = serial.Serial('/dev/ttyS6', 115200, timeout=1)

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Set the socket option to allow reusing the address
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Close any existing socket on port 12345
try:
    existing_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    existing_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    existing_sock.connect(('localhost', 12345))
    existing_sock.close()
    print("Closed existing socket on port 12345")
except OSError:
    pass

# Bind the socket to the specified port
server_address = ('localhost', 12300)
sock.bind(server_address)

running = True
while running:
    print('\nWaiting to receive message')
    data, address = sock.recvfrom(4096)

    try:
        # Convert data to integer or float
        position = struct.unpack('f', data)[0]

        # Debugging: Print received position
        print("Received position:", position)

        # Check if position is within range
        if -90 <= position <= 90:
            print("Moving servo to position:", position)

            # Build command for the actuator
            command = build_pos_command(position)

            # Send command to the actuator
            ser.write(bytearray(command))
            print("Command sent to actuator")
        else:
            print("Error: Angle must be between -90 and 90 degrees")
    except ValueError:
        print("Error: Received data is not a valid position")
        continue

# Close socket
sock.close()

'''
import socket
from volz_actuator import build_pos_command
import serial
import struct

# Initialize serial connection to the actuator
ser = serial.Serial('/dev/ttyS6', 115200, timeout=1)

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('localhost', 12345)
sock.bind(server_address)

while True:
    print('\nWaiting to receive message')
    data, address = sock.recvfrom(4096)

    try:
        # Convert data to integer or float
        position = struct.unpack('f', data)[0]
        print("Moving servo to position:", position)

        # Build command for the actuator
        command = build_pos_command(position)
        
        # Send command to the actuator
        ser.write(bytearray(command))
        print("Command sent to actuator")
    except ValueError:
        print("Received data is not a valid position")
'''