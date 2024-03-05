import socket
import struct
from StirlingNGI import *

def interact(ngi, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

    print(f"Sending data to port 11111")
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:
        """ RECEIVE FROM PORT 7004"""
        data, addr = ngi.rxSockStatus.recvfrom(4096)
        client.sendto(data, ('localhost', 11111))

        axis, pos, force, sw09, sw10, sw11, sw12 = ngi.decodeMsg10(data)
        print(f"Axis {axis} | Position {pos[0]}")
        # print(f"Force {force} | sw09 {sw09} | sw10 {sw10} | sw11 {sw11} | sw12 {sw12}")
        if axis == 0:
            print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
            # Convert to ratio to feed to xplane [-1, 1]
            pitchNorm = 2 * (pos[0] - ngi.PITCH_MIN) / (ngi.PITCH_MAX - ngi.PITCH_MIN) - 1
            if pitchNorm > 1:
                pitchNorm = 1.0
            elif pitchNorm < -1.0:
                pitchNorm = -1.0
        elif axis == 1:
            print(f"axis: roll | position: {pos[0]} | force: {force[0]}")
            rollNorm = 2 * (pos[0] - ngi.ROLL_MIN) / (ngi.ROLL_MAX - ngi.ROLL_MIN) - 1
            if rollNorm > 1:
                rollNorm = 1.0
            elif rollNorm < -1:
                rollNorm = -1.0

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
        # ngi.forceOffsetLoop()

        """ STIRLING INTERACTION """
        interact(ngi)
    except KeyboardInterrupt as e:
        print(e)
    finally:
        print("Data transmission terminated by user.")

if __name__ == '__main__':
    main()