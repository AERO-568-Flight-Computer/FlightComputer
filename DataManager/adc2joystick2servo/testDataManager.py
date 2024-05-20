import socket
import serial
import struct
import time
from time import sleep

# Data Aggregator: Receives data from the NGI and sends it to Servo.
# Translates hex data from servo to force in Newtons. Then converts to degrees for servo, sends command after time.

# Port 11111: Receives data from NGI
# Port 12300: Sends data to Servo

# Function to convert position to degrees
def convertPositionToDegrees(position):
    # Ensure position is within the range of -20 to 20
    if position < -20:
        position = -20
    elif position > 20:
        position = 20

    # Mapping the position range (-20 to 20) to the angle range (-90 to 90)
    # Using linear interpolation formula: angle = (position - min_pos) * (max_angle - min_angle) / (max_pos - min_pos) + min_angle
    min_pos = -20
    max_pos = 20
    min_angle = -45
    max_angle = 45
    
    angle = (position - min_pos) * (max_angle - min_angle) / (max_pos - min_pos) + min_angle

    return angle

# Function to return NGI pitch position
def main():
    # Create UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind socket to specified port
    server_address = ('localhost', 11111)
    sock.bind(server_address)

    running = True
    while running:
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)

        try:
            """ RECEIVE FROM PORT 7004"""

            
            if axis == 0:
                # print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
                pitchNorm = 2 * (pos[0] - PITCH_MIN) / (PITCH_MAX - PITCH_MIN) - 1
                if pitchNorm > 1:
                    pitchNorm = 1.0
                elif pitchNorm < -1.0:
                    pitchNorm = -1.0
                pitchPosition = pos[0]

            print("Position: ", pitchPosition)
            angle = convertPositionToDegrees(pitchPosition) # Convert Position to degrees

            # Create a socket object using UDP (not TCP)
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            # Convert the float to bytes, as we can only send bytes
            message_bytes = struct.pack('f', angle)
            client.sendto(message_bytes, ('localhost', 12300))
            print("Sending: ", angle, "to port 12300")

        except ValueError:
            print("Error: Received data is not valid.")

if __name__ == '__main__':
    main()