import socket
import threading
import numpy as np
import time
import json
import os


def setupPartitions(partitionInfo):

    # List of partition names
    partitionNames = tuple([partition["name"] for partition in partitionInfo])

    # List of ports
    receivePorts = tuple([partition["portSend"] for partition in partitionInfo])

    # List of send ports
    sendPorts = tuple([partition["portReceive"] for partition in partitionInfo])

    return partitionNames, receivePorts, sendPorts


def dataDecode(data, partNum):
    # First 2 bytes are the number of rows
    # next numRows * numFields * 8 bytes are the data

    numRows = int.from_bytes(data[0:2], byteorder='big')

    # Get the number of fields from the partitionInfo global variable
    numFields = len(partitionInfo[partNum]["sendDict"])

    # If the amount of data is not correct, error out
    if len(data) != 2 + numRows * numFields * 8:
        raise ValueError("Data length is not correct")

    # get the array
    return np.frombuffer(data[2:], dtype=np.float64).reshape(numRows, -1)


def handle_client(server, partNum):

    while True:
        try:
            # Supposedly, recvfrom is a blocking call, so i don't need the conditional
            # statement below. I will leave it for now.
            data, addr = server.recvfrom(65507)
            if not data:
                continue

            timeRecv = time.time()
            
            # Convert data to numpy array using our custom function
            newData = dataDecode(data, partNum)

            # Create time stamp array
            timeArray = np.full((newData.shape[0], 1), timeRecv, dtype=np.float64)

            # Acquire the lock before writing to the array
            with lockList[partNum]:
                # If the row is outside the current array, resize the array
                if rowList[partNum] >= cvtList[partNum].shape[0]:
                    new_size = cvtList[partNum].shape[0] + 1000
                    cvtList[partNum] = np.pad(cvtList[partNum], ((0, new_size), (0, 0)), mode='constant', constant_values=np.nan)
                    timeList[partNum] = np.pad(timeList[partNum], ((0, new_size), (0, 0)), mode='constant', constant_values=np.nan)

                # Write the data to the array
                cvtList[partNum][rowList[partNum]] = data
                timeList[partNum][rowList[partNum]] = time.time()
                rowList[partNum] += 1

        except KeyboardInterrupt:
            server.close()
            print(f"Receive connection to partition {partNum} closed")
            return
        

def serverListen(port, partNum):
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind(("0.0.0.0", port))
    print(f"[*] Listening on port {port}")

    handle_client(server, partNum)

def dataEncode(partNum):
    # Make sure to only call this function when a receiveDict exists
    # First 2 bytes are the number of rows
    # next numRows * numFields * 8 bytes are the data
    # This repeats for each partition that data is requested from

    sendPartitionList = []
    sendPartitionIndices = []
    # number of partitions that data is requested from (contained in partitionInfo)
    for i in range(len(partitionInfo[partNum]["receiveDict"])):
        # Find each distinct partition that data is requested from
        if partitionInfo[partNum]["receiveDict"][i][0] not in sendPartitionList:
            sendPartitionList.append(partitionInfo[partNum]["receiveDict"][i][0])
            
            # Get the index of the partition that data is requested from by finding that partition in partitionInfo
            sendPartitionIndices.append([j for j in range(len(partitionInfo)) if partitionInfo[j]["name"] == sendPartitionList[-1]])
    
    fieldIndices = []
    # Get the index of the fields that are requested
    for i in sendPartitionIndices:
        # TODO: Make sure this works
        fieldIndices.append([j for j in range(len(partitionInfo[i[0]]["sendDict"])) if partitionInfo[i[0]]["sendDict"][j] in partitionInfo[partNum]["receiveDict"]])


def sendData(sock, partNum):
    try:
        while True:
            # Send the data every 5 seconds
            time.sleep(5)
    except KeyboardInterrupt:
        sock.close()
        print(f"Connection to partition {partNum} closed")
  

def saveCVT(saveTime, numPartitions):
    # Most recent row saved for each partition
    recentRow = [0 for i in range(numPartitions)]
    while True:
        # Save the data every saveTime seconds
        time.sleep(saveTime)

        # List to store the most recent CVT and time array
        recentCVT = []
        recentTime = []

        # Save a local copy of each CVT and time array
        for i in range(numPartitions):
            with lockList[i]:
                if rowList[i] > recentRow[i]:
                    
                    # TODO: This might change if there are extra columns
                    recentCVT.append(cvtList[i][recentRow[i]:rowList[i]])
                    recentTime.append(timeList[i][recentRow[i]:rowList[i]])

                    # Update the most recent row saved
                    recentRow[i] = rowList[i]

                else:
                    recentCVT.append(0)
                    recentTime.append(0)
        
        # Save the local copy to a file
        for i in range(numPartitions):

            # I put zero when there is no new data to save
            if type(recentCVT[i]) != int:
                # Get the field names from partitionInfo global variable
                fieldnames = ",".join(list(partitionInfo[i]["sendDict"].values()))

                # Ensure that the DataAggRecords directory exists
                os.makedirs('DataAggRecords', exist_ok=True)

                # Check if the file exists
                if not os.path.isfile(f"DataAggRecords/CVT{i}.csv"):
                    # If the file doesn't exist, write the field names
                    with open(f"DataAggRecords/CVT{i}.csv", 'w') as f:
                        f.write(fieldnames + '\n')

                # Write the data
                with open(f"DataAggRecords/CVT{i}.csv", 'ab') as f:
                    np.savetxt(f, recentCVT[i], delimiter=",")

                # Do the same for the time file
                if not os.path.isfile(f"DataAggRecords/time{i}.csv"):
                    with open(f"DataAggRecords/time{i}.csv", 'w') as f:
                        f.write(fieldnames + '\n')

                with open(f"DataAggRecords/time{i}.csv", 'ab') as f:
                    np.savetxt(f, recentTime[i], delimiter=",")

        print("################################################")
        print("\n\n\n          Save attempted \n\n\n")
        print("################################################")


def main():

    global partitionInfo
    partitionInfo = 0

    # Load the setup file
    with open("setupV2.json") as f:
        partitionInfo = json.load(f)


    # IMPORTANT: Use partitionInfo as read-only. I did not set up locks for this data structure.

    # Get partition names, receive ports, and send ports
    # These should also be used read only
    # TODO: The receive ports and send ports have been swapped here to 
    # make the names from the perspective of the data aggregator. This is
    # getting confusing and I probably need to decide on a different convention.
    partitionNames, receivePorts, sendPorts = setupPartitions(partitionInfo)

    print("Partitions registered: " + str(partitionNames))

    print(receivePorts)

    print(sendPorts)

    # Set the number of partitions
    numPartitions = len(partitionInfo)

    # Create a lock for each partition
    global lockList
    global cvtList
    global timeList
    global rowList
    lockList = []
    cvtList = []
    timeList = []
    rowList = []


    # This list will hold an array for each partition that stores the data
    # An additional array for each partition will hold the time the data was received
    for i, port in enumerate(receivePorts):
        
        # Check that port is an int
        if type(port) == int:
            numFields = len(partitionInfo[i]["sendDict"])
            cvtList.append(np.full((100, numFields), np.nan, dtype=np.float64))

            timeList.append(np.full((100, 1), np.nan, dtype=np.float64))

            
            # Create a lock for each partition that is sending data
            lockList.append(threading.Lock())

            # Clear the numFields variable
            del numFields

        else:
            cvtList.append(0)
            timeList.append(0)
            lockList.append(0)

        # List of most recent row in CVT
        rowList.append(0)


    sendSockList = []
    # Setting ports to send data over
    # Create a socket for each partition that is sending data
    for i, port in enumerate(sendPorts):
        # Check that port is an int
        if type(port) == int:
            sendSockList.append(socket.socket(socket.AF_INET, socket.SOCK_DGRAM))
            # Print the port if successful
            print(f"Sender initiated: {port}\nTo: {partitionNames[i]}")
        else:
            sendSockList.append(0)


    # You were here last (keep it strong soldier) #######################
    # Next steps:
    # Think about sender implementation
    # Think about the save function (copilot thought of this one)
    # Unrelated (check the global interpreter lock as it applies to separate terminals)

    # Create a listener thread for each partition
    for i, port in enumerate(receivePorts):
        if type(port) == int:

            listener = threading.Thread(target=serverListen, args=(port, i))

            listener.start()

    # Send data to each partition
    for i, port in enumerate(sendPorts):
        if type(port) == int:
            
            sender = threading.Thread(target=sendData, args=(sendSockList[i], i))

            sender.start()

    try:
        # Save the data every so often to a file
        saveTime = 30 # [s]
        saveCVT(saveTime, numPartitions)
    except KeyboardInterrupt:
        # for i in range(numPartitions):
        #     print(cvtList[i])
        #     print(timeList[i])
        # TODO: Trigger all threads to close using event


        return


if __name__ == "__main__":
    main()


# TODO: Column number in sender needs checking