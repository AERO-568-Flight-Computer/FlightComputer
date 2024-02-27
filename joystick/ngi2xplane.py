import socket
import struct
from StirlingNGI import *
from XPlaneUdp import *


def rxStatus(ngi, xplane):
    """ SET UP CSV FOR LOGGING """
    now = datetime.now(timezone.utc)
    dt = now.strftime("%Y%m%d_%H%M%S_UTC")
    this_dir = os.path.dirname(os.path.realpath(__file__))
    filename = os.path.join(this_dir, dt + '.csv')

    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        interact(ngi, xplane, writer)


def interact(ngi, xplane, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

    print(f"Starting port 7004 data receive")
    while True:
        """ RECEIVE FROM PORT 7004"""
        data, addr = ngi.rxSockStatus.recvfrom(4096)
        axis, pos, force, sw09, sw10, sw11, sw12 = ngi.decodeMsg10(data)
        # print(f"Axis {axis} | Position {pos[0]}")
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
        # row = data + ", " + str(pitchNorm) + ", " + str(rollNorm)   #TODO
        # print(f"normalized pitch: {pitchNorm}| normalized roll: {rollNorm}")
        # print(f"switch 10: {sw10} | switch 12: {sw12}")

        """ RECEIVE FROM PORT 7005 """
        # data, addr = ngi.rxSockLimRot.recvfrom(4096)
        # axisMsg11, posCoords, negCoords = ngi.decodeMsg11(data)
        # print(f"Axis: {axisMsg11} | RX POS: {posCoords} | RX NEG: {negCoords}")

        """ INTERACT WITH XPLANE """
        # Send Pitch and Roll Commands
        xplane.writeDataRef('sim/joystick/yoke_pitch_ratio', val=pitchNorm)
        xplane.writeDataRef('sim/joystick/yoke_roll_ratio', val=rollNorm)

        # # Send Throttle Commands
        # if sw10 == 1 and throttle < 1:
        #     # throttle up
        #     throttle = xplane.throttleUp(throttle)
        # if sw12 == 1 and throttle > 0:
        #     # throttle down
        #     throttle = xplane.throttleDown(throttle)

        # Adjust Trim Based on Switches
        if sw10 == 1 and pitchTrimVal > -1:  # switch forward
            pitchTrimVal = xplane.pitchTrim(pitchTrimVal, -trimStep)
            # pitch trim down
        if sw12 == 1 and pitchTrimVal < 1:  # switch back
            pitchTrimVal = xplane.pitchTrim(pitchTrimVal, trimStep)
            # pitch trim up
        if sw09 == 1 and rollTrimVal > -1:   # switch left
            rollTrimVal = xplane.rollTrim(rollTrimVal, -trimStep)
        if sw11 == 1 and rollTrimVal < 1:   # switch right
            rollTrimVal = xplane.rollTrim(rollTrimVal, trimStep)

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, xplane, 'pitch', pos[0])
            adjustForce(ngi, xplane, 'roll', pos[0])
            count = 0
        count += 1

        # Write to csv file if writing is enabled and writer is provided
        # if writer:
        #     writer.writerow(row)


def adjustForce(ngi, xplane, axis, position, ias=None):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule
    ias = xplane.getDataRef(b"sim/flightmodel/position/indicated_airspeed")
    if ias < 20:
        ias = 20
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


def calcForce(airspeed):
    if airspeed < 20:
        airspeed = 10
    return pow(airspeed,1.7) / 150


def main():
    xplane = XPlaneUdp()
    xplane.setupJoystick()

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


        """ STIRLING AND XPLANE INTERACTION """
        # rxStatus(ngi, xplane)
        interact(ngi, xplane)

    except KeyboardInterrupt as e:
        print(e)
    finally:
        ngi.tearDown()


if __name__ == '__main__':
    main()
