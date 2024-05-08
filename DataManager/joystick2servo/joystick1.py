import socket
import struct
from NGIcalibration1 import *

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

def interact(ngi, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # rxSockStatus = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    while True:
        """ RECEIVE FROM PORT 7004"""

        print("Waiting to receive data")

        data, addr = ngi.rxSockStatus.recvfrom(4096)

        print(data)
        
        try:
            PITCH_MIN = -20.0
            PITCH_MAX = 20.0
            ROLL_MIN = -20.0
            ROLL_MAX = 20.0

            """ RECEIVE FROM PORT 7004"""
            axis, pos, force, sw09, sw10, sw11, sw12 = decodeMsg10(data)
            print(f"Axis {axis} | Position {pos[0]}")
            # print(f"Force {force} | sw09 {sw09} | sw10 {sw10} | sw11 {sw11} | sw12 {sw12}")
            if axis == 0:
                print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
                # Convert to ratio to feed to xplane [-1, 1]
                pitchNorm = 2 * (pos[0] - PITCH_MIN) / (PITCH_MAX - PITCH_MIN) - 1
                if pitchNorm > 1:
                    pitchNorm = 1.0
                elif pitchNorm < -1.0:
                    pitchNorm = -1.0
                pitchPosition = pos[0]
                return pitchPosition
            elif axis == 1:
                print(f"axis: roll | position: {pos[0]} | force: {force[0]}")
                rollNorm = 2 * (pos[0] - ROLL_MIN) / (ROLL_MAX - ROLL_MIN) - 1
                if rollNorm > 1:
                    rollNorm = 1.0
                elif rollNorm < -1:
                    rollNorm = -1.0
        except ValueError:
            print("Error: Received data is not valid.")

        print("Sending Position", pitchPosition, "to port 11112")
        client.sendto(data, ('localhost', 11111))
        client.sendto(pitchPosition, ('localhost', 11112))

def main():

    ngi = StirlingInceptor()

    try:

        """ IBIT """
        # ngi.IBIT()

        """ ACTIVATION """
        ngi.activate()

        """ ADJUST CALIBRATION FORCE OFFSET """
        sleep(2)
        ngi.configSetup()
        sleep(2)

        """ STIRLING INTERACTION """
        interact(ngi)

    except KeyboardInterrupt as e:
        print(e)
    finally:
        print("Data transmission terminated by user.")

if __name__ == '__main__':
    main()
    

