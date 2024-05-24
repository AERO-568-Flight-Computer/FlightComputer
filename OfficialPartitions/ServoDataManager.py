import socket
import serial
import struct
import time
from time import sleep

# Data Aggregator: Receives data from the NGI and sends it to Servo.
# Translates hex data from servo to force in Newtons. Then converts to degrees for servo, sends command after time.

# Port 11111: Receives pitch position from NGI
# Port 11112: Receives pitch trim up from NGI
# Port 11113: Receives pitch trim down from NGI
# Port 11114: Receives roll position from NGI
# Port 11115: Receives roll trim left from NGI
# Port 11116: Receives roll trim right from NGI
# Port 12300: Sends Pitch Angle to Servo
# Port 12301: Sends Roll Angle to Servo

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

# Function to update trim
delayInterval = 1
lastTrim_elv = time.time()
lastTrim_ail = time.time()
trimSum_elv = 0 # degrees
trimSum_ail = 0
maxTrimSum = 20 # degrees
minTrimSum = -20 # degrees

def updateTrim_elv(trimup, trimdwn):
    global trimSum_elv, lastTrim_elv
    current = time.time()
    if current - lastTrim_elv >= delayInterval:
        if trimup == 1 and trimSum_elv < maxTrimSum:
            trimSum_elv += 1
        if trimdwn == 1 and trimSum_elv > minTrimSum:
            trimSum_elv -= 1
    return trimSum_elv

def updateTrim_ail(trimlft, trimrht):
    global trimSum_ail, lastTrim_ail
    current = time.time()
    if current - lastTrim_ail >= delayInterval:
        if trimlft == 1 and trimSum_ail < maxTrimSum:
            trimSum_ail += 1
        if trimrht == 1 and trimSum_ail > minTrimSum:
            trimSum_ail -= 1
    return trimSum_ail

# Function to return NGI pitch position
def main():
    running = True
    while running:

        # Port 11111
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11111)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        pitchPosition = struct.unpack('f', data)[0]

        # Port 11112
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11112)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        trimup = struct.unpack('f', data)[0]

        # Port 11113
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11113)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        trimdwn = struct.unpack('f', data)[0]    

        # Port 11114
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11114)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        rollPosition = struct.unpack('f', data)[0]   

        # Port 11115
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11115)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        trimlft = struct.unpack('f', data)[0]

        # Port 11116
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('localhost', 11116)
        sock.bind(server_address)       
        print("Waiting to receive data")
        data, address = sock.recvfrom(4096)
        trimrht = struct.unpack('f', data)[0]

        try:
            PITCH_MIN = -20.0
            PITCH_MAX = 20.0
            ROLL_MIN = -20.0
            ROLL_MAX = 20.0
            
            print("Pitch Position: ", pitchPosition)
            elv_angle = convertPositionToDegrees(pitchPosition)
            print("Pitch Angle before trim: ", elv_angle)

            updateTrim_elv(trimup, trimdwn)
            elv_angle += trimSum_elv
            print("Pitch Angle after trim: ", elv_angle)

            print("Roll Position: ", rollPosition)
            ail_angle = convertPositionToDegrees(rollPosition)
            print("Roll Angle before trim: ", ail_angle)

            updateTrim_ail(trimlft, trimrht)
            ail_angle += trimSum_ail
            print("Roll Angle after trim: ", ail_angle)

            # Create a socket object using UDP
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            # Convert the float to bytes
            message_bytes1 = struct.pack('f', elv_angle)
            client.sendto(message_bytes1, ('localhost', 12300))
            print("Sending: ", elv_angle, "to port 12300")

            # Create a socket object using UDP
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            # Convert the float to bytes
            message_bytes2 = struct.pack('f', ail_angle)
            client.sendto(message_bytes2, ('localhost', 12301))
            print("Sending: ", ail_angle, "to port 12301")           

        except ValueError:
            print("Error: Received data is not valid.")

if __name__ == '__main__':
    main()