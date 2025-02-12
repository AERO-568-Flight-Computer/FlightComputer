#import serial
import random
import socket
import sys
import json
import time
from time import sleep
import numpy as np
import select
# import matplotlib.pyplot as plt
from DataProcessor import DataProcessor
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

# Function to convert position to degrees
def convertPositionToDegrees(position):
    # Ensure position is within the range of -20 to 20
    if position < -20:
        position = -20
    elif position > 20:
        position = 20

    # Mapping the position range (-20 to 20) to the angle range (-90 to 90)
    # Using linear interpolation formula: angle = (position - min_pos) * (max_angle - min_angle) / (max_pos - min_pos) + min_angle
    min_pos = -20
    max_pos = 20
    min_angle = -45
    max_angle = 45
    
    angle = (position - min_pos) * (max_angle - min_angle) / (max_pos - min_pos) + min_angle

    return angle

# Function to update trim
delayInterval = 1
lastTrim_elv = time.time()
lastTrim_ail = time.time()
trimSum_elv = 0 # degrees
trimSum_ail = 0
maxTrimSum = 20 # degrees
minTrimSum = -20 # degrees

def updateTrim_elv(trimup, trimdwn):
    global trimSum_elv, lastTrim_elv
    current = time.time()
    if current - lastTrim_elv >= delayInterval:
        if trimup == 1 and trimSum_elv < maxTrimSum:
            trimSum_elv += 1
        if trimdwn == 1 and trimSum_elv > minTrimSum:
            trimSum_elv -= 1
    return trimSum_elv

def updateTrim_ail(trimlft, trimrht):
    global trimSum_ail, lastTrim_ail
    current = time.time()
    if current - lastTrim_ail >= delayInterval:
        if trimlft == 1 and trimSum_ail < maxTrimSum:
            trimSum_ail += 1
        if trimrht == 1 and trimSum_ail > minTrimSum:
            trimSum_ail -= 1
    return trimSum_ail
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

    ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
    ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
    ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis),
                      (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))

# Recieves data from the NGI and sends it to the Data Manager through UDP.

def interact(ngi, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

        # Consider making filepath a command line argument
    name = "name1joystick"
    filepath = "joystick_DA_i.json"

    # Create an instance of the DataProcessor class specified by the name and filepath
    processor = DataProcessor(name, filepath)

    # Display the attributes of the processor
    print(processor.name)
    print(processor.portSend)
    print(processor.portReceive)

    rateInternal = 1000

    dataDictionaryList = [
        {
            "timeRec": None,
            "pitchCommand": None
        }
    ]

    timeRecReceived = None
    iasReceived = None

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    next_send_time = time.time() + 10  # Set the initial time to send data after 10 seconds

    while True:
        processor.receiveData()
        recentData = processor.getRecentData("fc_demo", 1)
        timeRecRecieved = recentData[0, 0]
        iasReceived = recentData[0, 1]
        ias = iasReceived  # Placeholder for IAS
        #ias = 20  # Placeholder for IAS

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch', ias)
            adjustForce(ngi, 'roll', ias)
            count = 0
        count += 1

        """ RECEIVE FROM PORT 7004"""

        print("Waiting to receive data")
        
        data, addr = ngi.rxSockStatus.recvfrom(4096)

        axis, pos, force, trimlft, trimup, trimrht, trimdwn = decodeMsg10(data)

        if axis == 0:
           # print(f"axis: pitch | position: {pos[0]} | force: {force[0]}")
            pitchPosition = pos[0]
            print("Pitch Position: ", pitchPosition)
            angle = convertPositionToDegrees(pitchPosition) # Convert Position to degrees
            #print("Pitch Angle before trim: ", angle)
            #updateTrim_elv(trimup, trimdwn)
            #angle += trimSum_elv
            #print("Pitch Angle after trim: ", angle)
        internaltime  = time.time() 
        
        dataDictionaryList[0]["timeRec"] = internaltime
        dataDictionaryList[0]["pitchCommand"] = angle
        
        print(dataDictionaryList)

        processor.sendData(dataDictionaryList)


def main():

    ngi = StirlingInceptor()

    try:

        """ IBIT """
        ngi.IBIT()

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
    

