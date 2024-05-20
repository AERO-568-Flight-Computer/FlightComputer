import FlightComputerClasses
from time import sleep
import threading
import json

# This is the only list you need to modify
# Enter your partitions as strings. These will be used as the partition names. 
# You must create a subclass of Partition in FlightComputerClasses.py for each partition
partitionList = [
    'JoystickPitch',
    'JoystickRoll',
    'JoystickYaw'
]

# Empty Dictionary to hold the various objects
partitionsDict = {}

# Iterate the list, creating partitions, threads, and starting the threads
for partition in partitionList:
    # Get the class based on the partition name
    class_name = partition[0].upper() + partition[1:]
    # Create the class type, but don't instantiate it
    class_ = getattr(FlightComputerClasses, class_name)
    # Create the partition object
    partitionsDict[partition + '_obj'] = class_(partition_name = partition)
    # Create the thread object
    partitionsDict[partition + '_thread'] = threading.Thread(target=partitionsDict[partition + '_obj'].run, name = partition)
    # Start each thread
    partitionsDict[partition + '_thread'].start()

# Create an instance of the Partition superclass, so you can communicate with the dataServer
partition = FlightComputerClasses.Partition(ip='localhost', port=12345, partition_name='Partition')

# Main loop to interact with the partitions
while True:
    print("Enter command (threads, partitions, get, exit):")
    command = input()
    if command == 'threads':
        # Print all currently running threads
        for thread in threading.enumerate():
            print(thread.name)
    elif command == 'partitions':
        # Get the currentPartitions dict from the server
        currentPartitions = json.loads(partition.partitions().replace("'", '"'))
        print('List of current registered partitions and last checkin time:')
        for key,value in currentPartitions.items():
            print(key, value)
    elif command == 'get':
        # Get the latest data for a particular partition
        print("Enter the specific partition to get the latest value from the DB:")
        specific_partition = input()
        result = partitionsDict[specific_partition + '_obj'].get()
        print(result)
    elif command == 'exit':
        # Gracefully stop all threads
        for partition in partitionList:
            partitionsDict[partition + '_obj'].stop()
            # Wait for each thread to finish
            partitionsDict[partition + '_thread'].join()
        break

