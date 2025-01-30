import struct
import json
import numpy as np
import binascii
import socket
import select

# Description: This class helps a partition to send and receive data to and from the data aggregator in the proper format.
# Inside the class, we have the JSON string that contains the information about the partitions. This
# JSON string can be used by any part of the code to get the information about the partitions.


class DataProcessor:

    JSON_STRING = '''
    [
        {
            "name": "partition1",
            "portSend": 12351,
            "portReceive": 12361,
            "rate": 1,
            "sendDict": {
                "0": "yaw",
                "1": "pitch",
                "2": "roll"
            },
            "receiveDict": {
                "0": ["partition1", "yaw"],
                "1": ["partition1", "pitch"],
                "2": ["partition1", "roll"],
                "3": ["partition2", "roll"]
            }
        },
        {
            "name": "name1",
            "portSend": 12353,
            "portReceive": 12363,
            "rate": 900,
            "sendDict":
                {
                    "0": "timeRec",
                    "1": "sineWave"
                },
            "receiveDict":
                {
                    "0": ["name1", "timeRec"],
                    "1": ["name1", "sineWave"]
                }
        },
        {
            "name": "partition2",
            "portSend": "FALSE",
            "portReceive": 12362,
            "rate": 1,
            "receiveDict": {
                "0": ["partition1", "pitch"]
            }
        },
        {
            "name": "partition3",
            "portSend": 12353,
            "portReceive": 12363,
            "rate": 1,
            "receiveDict": {
                "0": ["partition1", "roll"]
            }
        },
        {
            "name": "AirDC",
            "portSend": 12351,
            "portReceive": "FALSE",
            "rate": 1,
            "sendDict": {
                "0": "yaw",
                "1": "pitch",
                "2": "roll",
                "3": "militime",
                "4": "absPressure",
                "5": "absSenseTemp",
                "6": "diffPressure",
                "7": "diffSenseTemp",
                "8": "rearFlagAOA",
                "9": "frontFlagYaw"
            }
        }
    ]
    '''

    def __init__(self, partitionName):

        self.jsonData = json.loads(DataProcessor.JSON_STRING)
        self.partitionName = partitionName
        self.name = False
        self.portSend = False
        self.portReceive = False
        self.rate = False
        self.sendDict = False
        self.receiveDict = False
        self.dataPointsPerPartition = None   # Dictionary to store the number of data points per partition
        self.currentRow = None
        
        self.initVariables()
    
    def initVariables(self):

        # Iterate through the JSON data to find the partition with the same name as the input (this is the string 
        # that is passed to the constructor of the class)
        for partition in self.jsonData:
            if partition["name"] == self.partitionName:
                # Below, actions that happen when the partition is found

                # Assign the values from the JSON data to the class variables. Values are set to False if they are not
                # present in the JSON data
                self.name = partition["name"]
                self.portSend = partition.get("portSend", False)
                self.portReceive = partition.get("portReceive", False)
                self.rate = partition.get("rate", False)
                self.sendDict = partition.get("sendDict", False)
                self.receiveDict = partition.get("receiveDict", False)

                # If the partition needs to send data, initialize a socket for sending data
                if self.portSend:
                    self.sendSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

                # Initialize receive socket if portReceive is defined
                if self.portReceive:
                    self.receiveSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    self.receiveSock.bind(("localhost", self.portReceive))

                # If the partition needs to receive data, find the number of data points per partition
                if self.receiveDict:
                    dataPointsPerPartition = {}
                    self.currentRow = {}
                    for key, value in self.receiveDict.items():
                        partition = value[0]
                        if partition not in dataPointsPerPartition:
                            dataPointsPerPartition[partition] = 0
                            self.currentRow[partition] = 0
                        dataPointsPerPartition[partition] += 1

                self.numpyArraysDict = {}

                # Create a dictionary that stores the data received from other partitions
                for partition, numPoints in dataPointsPerPartition.items():
                    self.numpyArraysDict[partition] = np.zeros((2000, numPoints), dtype=np.float64)

                # Store the number of fields for each partition in a separate list
                self.receivePartitionFieldCount = [array.shape[1] for array in self.numpyArraysDict.values()]

                # Store the number of data points per partition
                self.dataPointsPerPartition = dataPointsPerPartition

                break
    
    # This method takes in a list of dictionaries, where each dictionary contains the data to be sent. The keys of the
    # dictionary should match the keys in the sendDict attribute of the partition. The method then sends the data over
    # UDP to the data aggregator.
    def sendData(self, dataDictionaryList):
        if not self.sendDict:
            return None
        
        numFields = len(self.sendDict)
        numRows = len(dataDictionaryList)
        dataArray = np.full((numRows, numFields), np.nan, dtype=np.float64)
        
        for row, dataDictionary in enumerate(dataDictionaryList):
            for key, value in self.sendDict.items():
                if value in dataDictionary:
                    index = int(key)
                    dataArray[row, index] = dataDictionary[value]
        
        dataBytes = dataArray.tobytes()

        # Create the packet to send
        packet = struct.pack('>H', numRows) + dataBytes

        # Send the packet over UDP
        self.sendSock.sendto(packet, ('localhost', self.portSend))



    def receiveData(self):
    
    # This method receives data from the data aggregator. It reads the data from the receive socket and stores it in a
    # numpy array, with each partition having one 2D array dedicated to it. The data is stored in a circular buffer, 
    # so that the most recent data is always available. The method reads all the available packets from the socket 
    # and processes them.

    # Open questions:
    # 1. Are there multiple numpy arrays to store data from different partitions?
    # 2. What happens when the data rate from one partition is higher than the data rate from another partition?

        if not self.receiveSock:
            return None

        while select.select([self.receiveSock], [], [], 0)[0]:
            data, addr = self.receiveSock.recvfrom(6055)

            byteOffset = 0
            partitionBytes = []

            for numColumns in self.receivePartitionFieldCount:
                numRowsBytes = data[byteOffset:byteOffset + 2]
                numRows = int.from_bytes(numRowsBytes, byteorder='big')
                byteOffset += 2

                payloadSize = numRows * numColumns * 8
                if byteOffset + payloadSize > len(data):
                    print("Error: Insufficient data in the packet")
                    break

                partitionData = data[byteOffset:byteOffset + payloadSize]
                partitionBytes.append(partitionData)
                byteOffset += payloadSize

            for i, (partitionName, numColumns) in enumerate(self.dataPointsPerPartition.items()):

                receivedData = np.frombuffer(partitionData, dtype=np.float64)
                receivedData = receivedData.reshape(-1, numColumns)

                currentRow = self.currentRow.get(partitionName, 0)
                
                # TODO: Handle the case where the received data is larger than the buffer size
                #       Also, don't hardcode the buffer size
                remainingSpace = 2000 - currentRow

                numRows = receivedData.shape[0]

                if numRows <= remainingSpace:
                    # If the received data fits within the remaining space, write it directly
                    self.numpyArraysDict[partitionName][currentRow:currentRow+numRows,:] = receivedData
                else:
                    # If the received data exceeds the remaining space, write in two parts
                    # First, write the data that fits within the remaining space
                    self.numpyArraysDict[partitionName][currentRow:] = receivedData[:remainingSpace]
                    
                    # Then, write the remaining data from the beginning of the array
                    self.numpyArraysDict[partitionName][:numRows-remainingSpace] = receivedData[remainingSpace:]

                # Update the current row for the partition
                self.currentRow[partitionName] = (currentRow + numRows) % 2000
        return

    def getRecentData(self, partitionName, numRows):
        if partitionName not in self.numpyArraysDict:
            raise ValueError(f"Partition '{partitionName}' not found.")

        currentRow = self.currentRow.get(partitionName, 0)
        numpyArray = self.numpyArraysDict[partitionName]
        numColumns = numpyArray.shape[1]

        if numRows > 2000:
            raise ValueError("Number of rows requested exceeds the array size.")

        if currentRow >= numRows:
            # If the current row is greater than or equal to the number of rows requested,
            # we can retrieve the data directly from the array
            recentData = numpyArray[currentRow - numRows:currentRow]
        else:
            # If the current row is less than the number of rows requested,
            # we need to retrieve data from the end of the array and the beginning
            dataFromEnd = numpyArray[2000 - (numRows - currentRow):]
            dataFromStart = numpyArray[:currentRow]
            recentData = np.vstack((dataFromEnd, dataFromStart))

        return recentData
    


# TODO:
# Set up the receive method to clear out the buffer the first time it is called
# Don't hardcode the buffer size anywhere