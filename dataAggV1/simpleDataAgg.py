import socket
import threading
import numpy as np
import time
import json
import os



def handle_client(server, column):

    while True:
        data, addr = server.recvfrom(1024)
        if not data:
            continue
        data = int(data.decode())
        print(f"Received: {data} \nFrom: {column} \nAt: {time.time()}")

        # Acquire the lock before writing to the array
        with lockList[column]:
            # If the row is outside the current array, resize the array
            if rowList[column] >= cvtList[column].shape[0]:
                new_size = cvtList[column].shape[0] + 100
                cvtList[column] = np.pad(cvtList[column], ((0, new_size), (0, 0)), mode='constant', constant_values=np.nan)
                timeList[column] = np.pad(timeList[column], ((0, new_size), (0, 0)), mode='constant', constant_values=np.nan)

            # Write the data to the array
            cvtList[column][rowList[column]] = data
            timeList[column][rowList[column]] = time.time()
            rowList[column] += 1


def server_listen(port, column):
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind(("0.0.0.0", port))
    print(f"[*] Listening on port {port}")

    try:
        handle_client(server, column)

    # When the program is stopped, close the server
    except KeyboardInterrupt:
        print("[*] Closing server")
        server.close()
        print("[*] Server closed")


def saveCVT(increment, numPartitions):
    # Most recent row saved for each partition
    recentRow = [0 for i in range(numPartitions)]
    while True:
        # Save the data every increment seconds
        time.sleep(increment)

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

                # Ensure that the DataAggRecords directory exists
                os.makedirs('DataAggRecords', exist_ok=True)

                with open(f"DataAggRecords/CVT{i}.csv", 'ab') as f:
                    np.savetxt(f, recentCVT[i], delimiter=",")
                with open(f"DataAggRecords/time{i}.csv", 'ab') as f:
                    np.savetxt(f, recentTime[i], delimiter=",")

        print("################################################")
        print("\n\n\n          Save attempted \n\n\n")
        print("################################################")
            
        


def main():

    # Load the setup file
    with open("setup.json") as f:
        setup = json.load(f)
    
    # Extract the ports
    ports = [partition["port"] for partition in setup["partitions"]]

    # Set the number of partitions
    numPartitions = len(ports)

    # Create a lock for each port
    global lockList
    global cvtList
    global timeList
    global rowList
    lockList = []
    cvtList = []
    timeList = []
    rowList = []

    for port in ports:
        lockList.append(threading.Lock())

    # This list will hold an array for each partition that stores the data
    # An additional array for each partition will hold the time the data was received
    for port in ports:
        # TODO: Add columns for partitions that collect more than one piece of data.
        cvtList.append(np.full((100, 1), np.nan))

        # No need to add more columns here, data from a given partition assumed to come at same time.
        # In the future, this could change
        timeList.append(np.full((100, 1), np.nan))


    # Keeps track of the row of the most recent data point. 
    # This will be used by the sender to know where to stop sending from the array.
    for port in ports:
        rowList.append(0)

    # Create a listener thread for each partition
    for i, port in enumerate(ports):
        listener = threading.Thread(target=server_listen, args=(port, i))
        listener.start()

    try:
        # Save the data every so often to a file
        increment = 30 # [s]
        saveCVT(increment, numPartitions)
    except KeyboardInterrupt:
        # for i in range(numPartitions):
        #     print(cvtList[i])
        #     print(timeList[i])
        return

if __name__ == "__main__":
    main()