import socket
import struct
from NGIcalibration import *
from time import sleep, time

# Recieves data from the NGI and sends it to the Data Manager through UDP.\

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

        try:
            PITCH_MIN = -20.0
            PITCH_MAX = 20.0
            ROLL_MIN = -20.0
            ROLL_MAX = 20.0

            """ RECEIVE FROM PORT 7004"""

            axis, pos, force, sw09, sw10, sw11, sw12 = decodeMsg10(data)
            # print(f"Axis {axis} | Position {pos[0]}")
            
            if axis == 0:
                # print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
                pitchNorm = 2 * (pos[0] - PITCH_MIN) / (PITCH_MAX - PITCH_MIN) - 1
                if pitchNorm > 1:
                    pitchNorm = 1.0
                elif pitchNorm < -1.0:
                    pitchNorm = -1.0
                pitchPosition = pos[0]

            # print(data)
            # Check if it's time to send data to port 11111
            if time() >= next_send_time:
                
                # Send Pitch Position Data
                pitchPositiondata = struct.pack('f', pitchPosition)
                client.sendto(pitchPositiondata, ('localhost', 11111))

                # Send Pitch Trim Data

                print(sw10)
                sw10data = struct.pack('f', sw10)
                client.sendto(sw10data, ('localhost', 11112))

                print(sw12)
                sw12data = struct.pack('f', sw12)
                client.sendto(sw12data, ('localhost', 11112))                

                next_send_time = time() + 3  # Update the next sending time

        except ValueError:
            print("Error: Received data is not valid.")


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
    

