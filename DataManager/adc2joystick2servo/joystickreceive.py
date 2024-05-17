import socket
import struct
from NGIcalibration import *
from time import sleep, time

# Recieves ias from ADC, converts ias to force, and sends it to the NGI through UDP.

def calcForce(airspeed):
    if airspeed < 5:
        airspeed = 5
    return airspeed / 4

def adjustForce(ngi, axis, position, ias= None):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule
    ias = 0
    if ias < 5:
        ias = 5
    force = calcForce(ias)

    if axis == 'pitch':
        scale = 1.5
    else:
        scale = 1

    # print(f"ias: {ias} | force: {force}")

    ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 2*scale*force], [15, 3*scale*force], [20, 4*scale*force]]
    ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 2*scale*force], [15, 3*scale*force], [20, 4*scale*force]]
    ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis),
                      (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))
    
def interact(ngi, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

    # print(f"Starting port 7004 data receive")
    while True:
        """ RECEIVE FROM PORT 7004"""
        data, addr = ngi.rxSockStatus.recvfrom(4096)
        axis, pos, force, sw09, sw10, sw11, sw12 = ngi.decodeMsg10(data)
        # print(f"Axis {axis} | Position {pos[0]}")
        if axis == 0:
            # print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
            # Convert to ratio to feed to xplane [-1, 1]
            pitchNorm = 2 * (pos[0] - ngi.PITCH_MIN) / (ngi.PITCH_MAX - ngi.PITCH_MIN) - 1
            if pitchNorm > 1:
                pitchNorm = 1.0
            elif pitchNorm < -1.0:
                pitchNorm = -1.0
        elif axis == 1:
            # print(f"axis: roll | position: {pos[0]} | force: {force[0]}")
            rollNorm = 2 * (pos[0] - ngi.ROLL_MIN) / (ngi.ROLL_MAX - ngi.ROLL_MIN) - 1
            if rollNorm > 1:
                rollNorm = 1.0
            elif rollNorm < -1:
                rollNorm = -1.0

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch', pos[0])
            adjustForce(ngi, 'roll', pos[0])
            count = 0
        count += 1

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

        interact(ngi)

    except KeyboardInterrupt as e:
        print(e)
    finally:
        ngi.tearDown()

if __name__ == '__main__':
    main()