import random
import socket
import sys
import json
import time
import numpy as np
import select
# import matplotlib.pyplot as plt
from DataProcessor import DataProcessor

# Create an instance of the DataProcessor class
processor = DataProcessor("name1")

# Access the instance variables and methods
print(processor.name)
print(processor.portSend)
print(processor.portReceive)

def main():

#     # Load the setup file
#     with open("sineWaveMulti.json") as f:
#         setup = json.load(f)

#     # Take in the name as a command line argument
#     if len(sys.argv) > 1:
#         name = sys.argv[1]
#     else:
#         raise ValueError("No name provided")
    
#     # TODO: Make this more compact
#     # Extract the relevant port
#     # portSend = setup[0]["portSend"]

#     # portReceive = setup[0]["portReceive"]

#     # rate = setup[0]["rate"]

#     # Read the file to get the ports and rate

    rateInternal = 1000

#     numPoints = 1

#     # Print the port
#     print("Port: ", portSend)

#     # Create a socket object to send data
#     sockSend = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#     # Create a socket object to receive data
#     sockReceive = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#     # Bind the socket to the port
#     sockReceive.bind(("localhost", portReceive))

#     internalDataStore = np.full((100,2), np.nan, dtype=np.float64)

#     recentRow = 0

#     externalDataStore = np.full((100,2), np.nan, dtype=np.float64)

#     recentRowExternal = 0

#     plt.ion()
#     # Generate random numbers and send them to the data aggregator
    try:
        for i in range(30000):

            time.sleep(1/rateInternal)
            # Generate 1 points of a sine wave
            internalTime = time.time()

            sinWave = np.sin(internalTime)
            
            dataDictionaryList = [
                {
                    "timeRec": internalTime,
                    "sinWave": sinWave,
                }
            ]

            processor.sendData(dataDictionaryList)
        
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        processor.sendSock.close()
        processor.receiveSock.close()
        print("Connections closed")

    # Save the external data and internal data csv file
    # Check the type of numpy array
    # np.savetxt(f"internalData_{name}.csv", , delimiter=",")
    # np.savetxt(f"externalData_{name}.csv", , delimiter=",")


#     # This stuff below doesn't work yet
#     # # Graph the data recorded as a sliding graph of time
#     # currentRow = 1

#     # line1, = plt.plot([], [], label="Internal")
#     # line2, = plt.plot([], [], label="External")

#     # while currentRow < internalDataStore.shape[0]:
#     #     numPlotPoints = 100
#     #     if currentRow >= numPlotPoints:
#     #         line1.set_data(internalDataStore[currentRow-numPlotPoints:currentRow, 0], internalDataStore[currentRow-numPlotPoints:currentRow, 1])
#     #         line2.set_data(externalDataStore[currentRow-numPlotPoints:currentRow, 0], externalDataStore[currentRow-numPlotPoints:currentRow, 1])
#     #     else:
#     #         line1.set_data(internalDataStore[:currentRow, 0], internalDataStore[:currentRow, 1])
#     #         line2.set_data(externalDataStore[:currentRow, 0], externalDataStore[:currentRow, 1])
#     #     plt.legend()
#     #     plt.draw()
#     #     plt.xlim([internalDataStore[currentRow - numPlotPoints, 0], internalDataStore[currentRow, 0]])
#     #     plt.pause(0.005)
#     #     currentRow += 20
    



    return


if __name__ == "__main__":
    main()