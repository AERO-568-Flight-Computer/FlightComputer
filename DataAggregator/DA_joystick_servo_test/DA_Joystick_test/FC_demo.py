
# flight computer process to send dummy airspeed to joystick, recieve trim and position







#dataProcessorSinTest for modification


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
    name = "fc_demo"
    filepath = "./joystick_DA_i.json"

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
            "Airspeed": None,
            "servo_command": None,
        }
    ]

    timeRecReceived = None
    sineWaveReceived = None

    try:
        for _ in range(90000):
            
            time.sleep(1/rateInternal)
            # Generate 1 point of a sine wave
            internalTime = time.time()
            Airspeed = 20 #+np.sin(internalTime)
            
            dataDictionaryList[0]["timeRec"] = internalTime
            dataDictionaryList[0]["Airspeed"] = Airspeed

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
                recentData = processor.getRecentData("name1joystick", 1)
                print("joystick data")
                print(recentData)
                #timeSent = dataDictionaryList[0]["timeRec"]
                #Airspeed = dataDictionaryList[0]["Airspeed"]
                timeRecReceived = recentData[0, 0]
                PitchCommandReceived = recentData[0, 1]


                #TODO WHY IS PITCH COMMAND PRINTING AIRSPEED


                #print(f"Time sent    : {timeSent}, Airspeed sent:     {Airspeed}")
                print(f"Time received: {timeRecReceived}, Pitch Command received: {PitchCommandReceived}")
                #recentData1 = processor.getRecentData("fc_demo", 1)
                #print("my own data")
                #print(recentData1)
        
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        processor.sendSock.close()
        processor.receiveSock.close()

        print()
        print("Connections closed")



    return


if __name__ == "__main__":
    main()





