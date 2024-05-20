import socket
import struct
from NGIcalibration1 import *
from time import sleep, time

# Recieves data from the NGI and sends it to the Data Manager through UDP.\

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

    next_send_time = time() + 10  # Set the initial time to send data after 10 seconds

    while True:

        """ RECEIVE FROM PORT 7004"""

        print("Waiting to receive data")
        data, addr = ngi.rxSockStatus.recvfrom(4096)

        # print(data)
        # Check if it's time to send data to port 11111
        if time() >= next_send_time:
            client.sendto(data, ('localhost', 11111))
            next_send_time = time() + 0  # Update the next sending time

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
    

