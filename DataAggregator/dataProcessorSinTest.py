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

    # Take in the name as a command line argument
    if len(sys.argv) > 1:
        name = sys.argv[1]
    else:
        raise ValueError("No name provided")

    # Create an instance of the DataProcessor class specified by the name
    processor = DataProcessor(name)

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

    try:
        for _ in range(30000):
            time.sleep(1/rateInternal)
            # Generate 1 point of a sine wave
            internalTime = time.time()
            sinWave = np.sin(internalTime)
            
            dataDictionaryList[0]["timeRec"] = internalTime
            dataDictionaryList[0]["sineWave"] = sinWave

            processor.sendData(dataDictionaryList)
        
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        processor.sendSock.close()
        processor.receiveSock.close()

        print()
        print("Connections closed")



    return


if __name__ == "__main__":
    main()