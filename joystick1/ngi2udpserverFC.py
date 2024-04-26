import socket
import serial
import struct

def decodeMsg10(msg):
    
    # Ensure the message length matches the expected length for decoding
    if len(msg) != 48:
        raise ValueError("Invalid message length. Expected 48 bytes.")

    # Unpack each field according to the provided format
    msgId = msg[0]
    axis = msg[1]
    inceptorNumber = msg[2]
    status = struct.unpack("I", msg[3:7])[0]  # Assuming status is a 4-byte unsigned integer
    pos = struct.unpack("f", msg[7:11])[0]
    force = struct.unpack("f", msg[11:15])[0]
    motorDemand = struct.unpack("f", msg[15:19])[0]
    switchState1 = struct.unpack("I", msg[19:23])[0]
    switch09 = (msg[20] >> 0) & 1  # switch left
    switch10 = (msg[20] >> 1) & 1  # switch forward
    switch11 = (msg[20] >> 2) & 1  # switch right
    switch12 = (msg[20] >> 3) & 1  # switch back
    switchState2 = struct.unpack("I", msg[23:27])[0]
    analogueSwitch1 = struct.unpack("f", msg[27:31])[0]
    analogueSwitch2 = struct.unpack("f", msg[31:35])[0]
    analogueSwitch3 = struct.unpack("f", msg[35:39])[0]
    ver = struct.unpack("f", msg[39:43])[0]
    rawForceSensorOut = struct.unpack("f", msg[43:47])[0]

    return axis, pos, force, switch09, switch10, switch11, switch12

def main():
    # Initialize serial connection
    # ser = serial.Serial('/dev/ttyS6',115200,timeout=1)

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
            axis, pos, force, sw09, sw10, sw11, sw12 = decodeMsg10(data)

            #print(pos)
            #print(force)

            print(f"Axis {axis} | Position {pos}")
            # print(f"Force {force} | sw09 {sw09} | sw10 {sw10} | sw11 {sw11} | sw12 {sw12}")
            if axis == 0:
                print(f"axis: pitch | position: {pos} | force: {force}")
                # Convert to ratio to feed to xplane [-1, 1]
                pitchNorm = 2 * (pos - PITCH_MIN) / (PITCH_MAX - PITCH_MIN) - 1
                if pitchNorm > 1:
                    pitchNorm = 1.0
                elif pitchNorm < -1.0:
                    pitchNorm = -1.0
            elif axis == 1:
                print(f"axis: roll | position: {pos} | force: {force}")
                rollNorm = 2 * (pos - ROLL_MIN) / (ROLL_MAX - ROLL_MIN) - 1
                if rollNorm > 1:
                    rollNorm = 1.0
                elif rollNorm < -1:
                    rollNorm = -1.0
        except ValueError:
            print("Error: Received data is not valid.")

if __name__ == '__main__':
    main()