import socket
import struct
from NGIcalibration import *
from time import sleep, time

# Recieves ias from ADC, converts ias to force, and sends it to the NGI through UDP.

# Calculates force based on speed
def calcForce(airspeed):
    if airspeed < 5:
        airspeed = 5
    return airspeed / 4

# Sends force value to the NGI
def adjustForce(ngi, axis, ias= None):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule

    # Recieve IAS from ADC
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
    
# Calls functions to interact with the NGI
def interact(ngi, writer=None):
    while True:
        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch')
            adjustForce(ngi, 'roll')
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