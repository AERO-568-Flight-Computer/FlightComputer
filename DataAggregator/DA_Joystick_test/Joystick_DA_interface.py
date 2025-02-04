#joystick to data aggregator using data processor class





import random
import socket
import sys
import json
import time
import numpy as np
import select
# import matplotlib.pyplot as plt
from DataProcessor import DataProcessor
import struct
from NGIcalibration1 import *
#from time import sleep, time




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

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #Replace with DA integration 
   
    # rxSockStatus = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    next_send_time = time() + 10  # Set the initial time to send data after 10 seconds

    while True:
        
        ias = 20  # Placeholder for IAS

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch', ias)
            adjustForce(ngi, 'roll', ias)
            count = 0
        count += 1

        """ RECEIVE FROM PORT 7004"""

        print("Waiting to receive data")
        
        data, addr = ngi.rxSockStatus.recvfrom(4096)
        # client.sendto( 1  , ('localhost', 22222))
        # print(data)
        # Check if it's time to send data to port 11111
        if time() >= next_send_time:
            client.sendto(data, ('localhost', 11111))
            next_send_time = time() + 0  # Update the next sending time
    
# Description dataProcessorSinTest.py

# Give the name corresponding to a partition listed in the JSON text
# as the first command line argument. Use ctrl C to properly stop 
# this program properly.

def main():

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
            #"timeRec": None,
            #"sineWave": None,
        }
    ]

    #timeRecReceived = None
    #sineWaveReceived = None


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
        for _ in range(30000):
            """ STIRLING INTERACTION """
            interact(ngi)  
            
            if _ % 10 == 0:

                # Use the method to get the data for partition "name1" and one row, i.e. the most recent point of data
                # The first argument is the name of the partition to get data from, it must have been
                # listed in the JSON text as a receive from partition for the current partition
                # The second argument is the number of rows to get, it will always include the most recent row
                
                #TODO replace with joystick com data

                recentData = processor.getRecentData("name1", 1)

                timeSent = dataDictionaryList[0]["timeRec"]
                sineSent = dataDictionaryList[0]["sineWave"]
                timeRecReceived = recentData[0, 0]
                sineWaveReceived = recentData[0, 1]

                print(f"Time sent    : {timeSent}, Sine sent:     {sineSent}")
                print(f"Time received: {timeRecReceived}, Sine received: {sineWaveReceived}")

        
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        processor.sendSock.close()
        processor.receiveSock.close()

        print()
        print("Connections closed")



    return


if __name__ == "__main__":
    main()