import socket
import struct
from NGIcalibration import *
from time import sleep, time

# Recieves data from the NGI and sends it to the Data Manager through UDP. Also recieves IAS from
# the ADC and sends force to the joystick.

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

# Calculates force based on speed
def calcForce(airspeed):
    if airspeed < 5:
        airspeed = 5
    return airspeed / 4

# Sends force value to the NGI
def adjustForce(ngi, axis, ias):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule

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

    next_send_time = time() + 10  # Set the initial time to send data after 10 seconds

    while True:

        ias = 130  # Placeholder for IAS

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch', ias)
            adjustForce(ngi, 'roll', ias)
            count = 0
        count += 1

        """ RECEIVE FROM PORT 7004"""
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        data, addr = ngi.rxSockStatus.recvfrom(4096)

        try:
            PITCH_MIN = -20.0
            PITCH_MAX = 20.0
            ROLL_MIN = -20.0
            ROLL_MAX = 20.0

            """ RECEIVE FROM PORT 7004"""
            axis, pos, force, sw09, sw10, sw11, sw12 = decodeMsg10(data)
            
            if axis == 0:
                pitchNorm = 2 * (pos[0] - PITCH_MIN) / (PITCH_MAX - PITCH_MIN) - 1
                if pitchNorm > 1:
                    pitchNorm = 1.0
                elif pitchNorm < -1.0:
                    pitchNorm = -1.0
                pitchPosition = pos[0]
            elif axis == 1:
                rollNorm = 2 * (pos[0] - ngi.ROLL_MIN) / (ngi.ROLL_MAX - ngi.ROLL_MIN) - 1
                if rollNorm > 1:
                    rollNorm = 1.0
                elif rollNorm < -1:
                    rollNorm = -1.0
                rollPosition = pos[0]    
            # print(data)

            # Check if it's time to send data
            if time() >= next_send_time:
                
                # Send Pitch Position Data
                pitchPositiondata = struct.pack('f', pitchPosition)
                client.sendto(pitchPositiondata, ('localhost', 11111))

                # Send Pitch Trim Data
                print(f"Forward: {sw10}")
                sw10data = struct.pack('f', sw10) # Stick Forward
                client.sendto(sw10data, ('localhost', 11112))

                print(f"Backward: {sw12}")
                sw12data = struct.pack('f', sw12) # Stick Back
                client.sendto(sw12data, ('localhost', 11113))   

                # Send Roll Position Data             
                rollPositiondata = struct.pack('f', rollPosition)
                client.sendto(rollPositiondata, ('localhost', 11114))

                # Send Roll Trim Data
                print(f"Forward: {sw09}")
                sw09data = struct.pack('f', sw09) # Stick Left
                client.sendto(sw09data, ('localhost', 11115))

                print(f"Backward: {sw11}")
                sw11data = struct.pack('f', sw11) # Stick Right
                client.sendto(sw11data, ('localhost', 11116))                 

                next_send_time = time() + 1  # Update the next sending time

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
    

