import socket
import serial
import struct
import time
from time import sleep

# Data Aggregator: Receives data from the NGI and sends it to Servo.
# Translates hex data from servo to force in Newtons. Then converts to degrees for servo, sends command after time.

# Port 11111: Receives data from NGI
# Port 12300: Sends data to Servo

# Translate hex data from servo to force in Newtons
def decodeMsg10(msg):
    # TODO: make this self.msg10.msgId, etc?
    msgId = msg[0]
    axis = msg[1]
    inceptorNumber = msg[2]
    # status = struct.unpack("L", msg[4:8])  # TODO: further unpack each bit
    # status = struct.unpack("I", msg[4:8])  # Assuming status is a 4-byte unsigned integer
    pos = struct.unpack("f", msg[8:12])
    force = struct.unpack("f", msg[12:16])
    motorDemand = struct.unpack("f", msg[16:20])
    # switchState1 = struct.unpack("L", msg[20:24])
    switch09 = (msg[21] >> 0) & 1  # switch left
    switch10 = (msg[21] >> 1) & 1  # switch forward
    switch11 = (msg[21] >> 2) & 1  # switch right
    switch12 = (msg[21] >> 3) & 1  # switch back
    # switchState2 = struct.unpack("L", msg[24:28])
    analogueSwitch1 = struct.unpack("f", msg[28:32])
    analogueSwitch2 = struct.unpack("f", msg[32:36])
    analogueSwitch3 = struct.unpack("f", msg[36:40])
    ver = struct.unpack("f", msg[40:44])
    rawForceSensorOut = struct.unpack("f", msg[44:48])

    return axis, pos, force, switch09, switch10, switch11, switch12

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
            PITCH_MIN = -20.0
            PITCH_MAX = 20.0
            ROLL_MIN = -20.0
            ROLL_MAX = 20.0

            """ RECEIVE FROM PORT 7004"""

            axis, pos, force, trimlft, trimup, trimrht, trimdwn = decodeMsg10(data)
            # print(f"Axis {axis} | Position {pos[0]}")
            
            if axis == 0:
                # print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
                pitchPosition = pos[0]
                print("Pitch Position: ", pitchPosition)
                angle = convertPositionToDegrees(pitchPosition) # Convert Position to degrees
                #print("Pitch Angle before trim: ", angle)

                #updateTrim_elv(trimup, trimdwn)
                #angle += trimSum_elv
                #print("Pitch Angle after trim: ", angle)

            if axis == 1:
                # print(f"axis: roll | position: {pos[0]} | force: {force[0]}")
                rollPosition = pos[0]
                print("Roll Position: ", rollPosition)
                ail_angle = convertPositionToDegrees(rollPosition)
               # print("Roll Angle before trim: ", ail_angle)

                #updateTrim_ail(trimlft, trimrht)
                #ail_angle += trimSum_ail
                #print("Roll Angle after trim: ", ail_angle)

            # Create a socket object using UDP (not TCP)
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            # Convert the float to bytes, as we can only send bytes
            message_bytes = struct.pack('fff', angle, trimup, trimdwn) #trimlft, trimrht)
            client.sendto(message_bytes, ('localhost', 12300)) 
            print("Sending: ", angle, "to port 12300")

        except ValueError:
            print("Error: Received data is not valid.")

if __name__ == '__main__':
    main()