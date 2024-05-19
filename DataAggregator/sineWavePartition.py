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
    numFields = 2

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

    numPoints = 1

    # Print the port
    print("Port: ", portSend)

    # Create a socket object to send data
    sockSend = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Create a socket object to receive data
    sockReceive = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    sockReceive.bind(("localhost", portReceive))

    internalDataStore = np.full((100,2), np.nan, dtype=np.float64)

    recentRow = 0

    externalDataStore = np.full((100,2), np.nan, dtype=np.float64)

    recentRowExternal = 0

    plt.ion()
    # Generate random numbers and send them to the data aggregator
    try:
        for i in range(30000):

            # Attempt to receive data while it is available
            while select.select([sockReceive], [], [], 0)[0]:
                data, addr = sockReceive.recvfrom(65000)

                # Decode the data
                externalData, numRows = dataDecode(data)

                # Every 500 iterations, print length of data

                if i % 500 == 0:
                    print("Length of data: ", len(data))

                while recentRowExternal + numRows > externalDataStore.shape[0]:
                    bufferArray = np.full((externalDataStore.shape[0],2), np.nan, dtype=np.float64)
                    externalDataStore = np.concatenate((externalDataStore, bufferArray), axis=0)

                # Store the data
                externalDataStore[recentRowExternal:recentRowExternal+numRows] = externalData
                # Change the first column to be the time received
                externalDataStore[recentRowExternal:recentRowExternal+numRows, 0] = time.time()
                recentRowExternal += numRows
            time.sleep(1/rateInternal)
            # Generate 10 points of a sine wave
            for i in range(numPoints):
                internalTime = time.time()
                
                if recentRow >= internalDataStore.shape[0]:
                    bufferArray = np.full((internalDataStore.shape[0],2), np.nan, dtype=np.float64)
                    internalDataStore = np.concatenate((internalDataStore, bufferArray), axis=0)
                
                internalDataStore[recentRow, 1] = np.sin(internalTime)
                internalDataStore[recentRow, 0] = internalTime
                recentRow += 1
                

            # Format data as 16 bit unsigned integers, array
            dataToSend = b''

            dataToSend += numPoints.to_bytes(2, byteorder='big')

            dataToSend += internalDataStore[recentRow-numPoints:recentRow].tobytes()

            # print("Sent data")
            # print(internalDataStore[recentRow-numPoints:recentRow])
                
            sockSend.sendto(dataToSend, ("localhost", portSend))

            numPlotPoints = 1000

            # if recentRow > numPlotPoints:
            #     plt.plot(internalDataStore[recentRow-numPlotPoints:recentRow], label="Internal")
            # if recentRowExternal > numPlotPoints:
            #     plt.plot(externalDataStore[recentRowExternal-numPlotPoints:recentRowExternal], label="External")
            # plt.legend()
            # plt.draw()  # Draw the plot
            # plt.pause(0.001)  # Pause for a short period (this also allows the plot to update)
            # plt.gca().clear()  # Clear the axes for the next iteration
            
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        sockSend.close()
        sockReceive.close()
        plt.close()
        print("Connections closed")

    # Save the external data and internal data csv file
    # Check the type of numpy array
    print(type(internalDataStore[1000, 1]))
    print(type(externalDataStore[1000, 0]))
    np.savetxt("internalData.csv", internalDataStore, delimiter=",")
    np.savetxt("externalData.csv", externalDataStore, delimiter=",")

    # Graph the data recorded as a sliding graph of time
    currentRow = 1

    line1, = plt.plot([], [], label="Internal")
    line2, = plt.plot([], [], label="External")

    while currentRow < internalDataStore.shape[0]:
        numPlotPoints = 100
        if currentRow >= numPlotPoints:
            line1.set_data(internalDataStore[currentRow-numPlotPoints:currentRow, 0], internalDataStore[currentRow-numPlotPoints:currentRow, 1])
            line2.set_data(externalDataStore[currentRow-numPlotPoints:currentRow, 0], externalDataStore[currentRow-numPlotPoints:currentRow, 1])
        else:
            line1.set_data(internalDataStore[:currentRow, 0], internalDataStore[:currentRow, 1])
            line2.set_data(externalDataStore[:currentRow, 0], externalDataStore[:currentRow, 1])
        plt.legend()
        plt.draw()
        plt.xlim([internalDataStore[currentRow - numPlotPoints, 0], internalDataStore[currentRow, 0]])
        plt.pause(0.005)
        currentRow += 20
    



    return


if __name__ == "__main__":
    main()
