import socket
import struct
from NGIcalibration import *

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
        print(f"Sending data to port 11111")

        client.sendto(data, ('localhost', 11111))

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