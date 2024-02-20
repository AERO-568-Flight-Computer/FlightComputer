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
