import random
import socket
import sys
import json
import time
import numpy as np
import select
import matplotlib.pyplot as plt


def dataDecode(data):
    # First 2 bytes are the number of rows
    # next numRows * numFields * 8 bytes are the data

    numRows = int.from_bytes(data[0:2], byteorder='big')

    # Get the number of fields from the partitionInfo global variable
    numFields = 1

    # If the amount of data is not correct, error out
    if len(data) != 2 + numRows * numFields * 8:
        raise ValueError("Data length is not correct")

    # get the array
    return np.frombuffer(data[2:], dtype=np.float64).reshape(numRows, -1), numRows


def main():

    # Load the setup file
    with open("sineWaveTest.json") as f:
        setup = json.load(f)

    name = "sineWavePartition"
    
    # TODO: Make this more compact
    # Extract the relevant port
    portSend = setup[0]["portSend"]

    portReceive = setup[0]["portReceive"]

    rate = setup[0]["rate"]

    rateInternal = 1000

    numPoints = int(rateInternal/rate)

    # Print the port
    print("Port: ", portSend)

    # Create a socket object to send data
    sockSend = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Create a socket object to receive data
    sockReceive = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    sockReceive.bind(("localhost", portReceive))

    internalDataStore = np.full((100,1), np.nan, dtype=np.float64)

    recentRow = 0

    externalDataStore = np.full((100,1), np.nan, dtype=np.float64)

    recentRowExternal = 0

    plt.ion()
    # Generate random numbers and send them to the data aggregator
    try:
        while True:

            # Attempt to receive data while it is available
            while select.select([sockReceive], [], [], 0)[0]:
                data, addr = sockReceive.recvfrom(65000)
                print("Received data")
                print(len(data))

                # Decode the data
                externalData, numRows = dataDecode(data)

                while recentRowExternal + numRows > externalDataStore.shape[0]:
                    joe = np.full((externalDataStore.shape[0],1), np.nan, dtype=np.float64)
                    externalDataStore = np.concatenate((externalDataStore, joe), axis=0)

                # Store the data
                externalDataStore[recentRowExternal:recentRowExternal+numRows] = externalData
                recentRowExternal += numRows
            
            # Generate 10 points of a sine wave
            for i in range(numPoints):
                internalTime = time.time()
                
                if recentRow >= internalDataStore.shape[0]:
                    joe = np.full((internalDataStore.shape[0],1), np.nan, dtype=np.float64)
                    internalDataStore = np.concatenate((internalDataStore, joe), axis=0)
                
                internalDataStore[recentRow] = np.sin(internalTime)
                recentRow += 1
                time.sleep(1/rateInternal)

            # Format data as 16 bit unsigned integers, array
            dataToSend = b''

            dataToSend += numPoints.to_bytes(2, byteorder='big')

            dataToSend += internalDataStore[recentRow-numPoints:recentRow].tobytes()

            # print("Sent data")
            # print(internalDataStore[recentRow-numPoints:recentRow])
                
            sockSend.sendto(dataToSend, ("localhost", portSend))

            numPlotPoints = 1000

            if recentRow > numPlotPoints:
                plt.plot(internalDataStore[recentRow-numPlotPoints:recentRow], label="Internal")
            if recentRowExternal > numPlotPoints:
                plt.plot(externalDataStore[recentRowExternal-numPlotPoints:recentRowExternal], label="External")
            plt.legend()
            plt.draw()  # Draw the plot
            plt.pause(0.001)  # Pause for a short period (this also allows the plot to update)
            plt.gca().clear()  # Clear the axes for the next iteration
            
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        sockSend.close()
        sockReceive.close()
        plt.close()
        print("Connections closed")

    return


if __name__ == "__main__":
    main()
