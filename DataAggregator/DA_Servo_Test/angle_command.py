import random
import socket
import sys
import json
import time
import numpy as np
import select
# import matplotlib.pyplot as plt
from DataProcessor import DataProcessor

# Description

# Give the name corresponding to a partition listed in the JSON text
# as the first command line argument. Use ctrl C to properly stop 
# this program properly.

def main():

    # Consider making filepath a command line argument
    name = "angle_command"
    filepath = "./servo_DA_test.json"

    # Create an instance of the DataProcessor class specified by the name and filepath
    processor = DataProcessor(name, filepath)

    # Display the attributes of the processor
    print(processor.name)
    print(processor.portSend)
    print(processor.portReceive)

    rateInternal = 0.5

    dataDictionaryList = [
        {
            "timeRec": None,
            "angle": None,
        }
    ]

    timeRecReceived = None
    angle = None

    try:
        for _ in range(30000):
            time.sleep(1/rateInternal)
            internalTime = time.time()
            angle = random.randint(-50, 50)
            
            dataDictionaryList[0]["timeRec"] = internalTime
            dataDictionaryList[0]["angle"] = angle

            processor.sendData(dataDictionaryList)

            # Receive data
            processor.receiveData()

            # TODO: Only do this if data is received
            # Print the data every 10 iterations
            if _ % 10 == 0:

                # Use the method to get the data for partition "name1" and one row, i.e. the most recent point of data
                # The first argument is the name of the partition to get data from, it must have been
                # listed in the JSON text as a receive from partition for the current partition
                # The second argument is the number of rows to get, it will always include the most recent row
                recentData = processor.getRecentData("servo_demo", 1)
                print("Servo Data")
                print(recentData)
                #timeSent = dataDictionaryList[0]["timeRec"]
                #Airspeed = dataDictionaryList[0]["Airspeed"]
                timeRecReceived = recentData[0, 0]
                PositionCommandReceived = recentData[0, 1]
                ClutchStatusReceived = recentData[0, 2]

                print(f"Time received: {timeRecReceived}, Position Command received: {PositionCommandReceived}, Clutch Status received: {ClutchStatusReceived}")
        
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        processor.sendSock.close()
        processor.receiveSock.close()

        print()
        print("Connections closed")
    return

if __name__ == "__main__":
    main()