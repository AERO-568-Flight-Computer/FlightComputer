import struct
import json
import numpy as np
import binascii
import socket

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
        
        self.initVariables()
    
    def initVariables(self):
        for partition in self.jsonData:
            if partition["name"] == self.partitionName:
                self.name = partition["name"]
                self.portSend = partition.get("portSend", False)
                self.portReceive = partition.get("portReceive", False)
                self.rate = partition.get("rate", False)
                self.sendDict = partition.get("sendDict", False)
                self.receiveDict = partition.get("receiveDict", False)

                if self.portSend:
                    self.sendSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

                # Initialize receive socket if portReceive is defined
                if self.portReceive:
                    self.receiveSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    self.receiveSock.bind(("localhost", self.portReceive))

                if self.receiveDict:
                    dataPointsPerPartition = {}
                    for key, value in self.receiveDict.items():
                        partition = value[0]
                        if partition not in dataPointsPerPartition:
                            dataPointsPerPartition[partition] = 0
                        dataPointsPerPartition[partition] += 1

                    self.dataPointsPerPartition = list(dataPointsPerPartition.values())

                break
    
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
        self.sock.sendto(packet, ('localhost', self.portSend))

    def receiveData(self):
        if not self.receiveSock:
            return None

        while select.select([self.receiveSock], [], [], 0)[0]:
            data, addr = self.receiveSock.recvfrom(6055)

            byteOffset = 0
            partitionBytes = []

            for numColumns in self.dataPointsPerPartitionArray:
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

            for partitionData in partitionBytes:

                receivedData = np.frombuffer(dataArray, dtype=np.float64)

                # Reshape the array to have numRows rows and numFields columns
                receivedData = receivedData.reshape(numRows, numFields)

        return receivedData, numRows

        
    def close(self):
        



# processor = DataProcessor("AirDC")
# print(processor.name)
# print(processor.portSend)
# print(processor.portReceive)
# print(processor.rate)
# print(processor.sendDict)
# print(processor.receiveDict)

# dataDictionaryList = [
#     {
#         "yaw": 10.5,
#         "pitch": 5.2,
#         "roll": 2.8,
#         "militime": 1000,
#         "absPressure": 101325.0,
#         "absSenseTemp": 25.0,
#         "diffPressure": 50.0,
#         "diffSenseTemp": 2.0,
#         "rearFlagAOA": 3.0,
#         "frontFlagYaw": 1.0
#     }
# ]

# dataBytes = processor.sendData(dataDictionaryList)
# print(dataBytes)