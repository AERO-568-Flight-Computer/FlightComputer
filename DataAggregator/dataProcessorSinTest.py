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
    name = "name1"
    filepath = "./sineWaveMulti.json"

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
            "sineWave": None,
        }
    ]

    timeRecReceived = None
    sineWaveReceived = None

    try:
        for _ in range(30000):
            time.sleep(1/rateInternal)
            # Generate 1 point of a sine wave
            internalTime = time.time()
            sinWave = np.sin(internalTime)
            
            dataDictionaryList[0]["timeRec"] = internalTime
            dataDictionaryList[0]["sineWave"] = sinWave

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
                recentData = processor.getRecentData("name1", 1)

                #timeSent = dataDictionaryList[0]["timeRec"]
                #sineSent = dataDictionaryList[0]["sineWave"]
                #timeRecReceived = recentData[0, 0]
                #sineWaveReceived = recentData[0, 1]


               # print(f"Time sent    : {timeSent}, Sine sent:     {sineSent}")
               # print(f"Time received: {timeRecReceived}, Sine received: {sineWaveReceived}")

                recentData = processor.getRecentData("name5", 1)


                timeSent = dataDictionaryList[0]["timeRec"]
                sineSent = dataDictionaryList[0]["sineWave"]
                timeRecReceived = recentData[0, 0]
                sineWaveReceived = recentData[0, 1]



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