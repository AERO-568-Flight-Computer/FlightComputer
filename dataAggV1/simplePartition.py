
# This function generates random numbers and sends them to the data aggregator

# Pass in the name of the partition, the rate at which to send data, and the setup file name
# As command-line arguments

import random
import socket
import sys
import json
import time

def main():

    # Check if the correct number of arguments was provided
    if len(sys.argv) != 3:
        print("Usage: python simplePartition.py <name> <setup_file>")
        return

    # Get the name and rate from the command-line arguments
    name = sys.argv[1]
    setupFile = sys.argv[2]

    # Print the name and rate
    print("Name: ", name)

    # Load the setup file
    with open(setupFile) as f:
        setup = json.load(f)
    
    # TODO: Make this more compact
    # Extract the relevant port
    port = [partition["port"] for partition in setup["partitions"] if partition["name"] == name][0]

    # Get low end of range
    low = [partition["rangeLow"] for partition in setup["partitions"] if partition["name"] == name][0]

    # Get high end of range
    high = [partition["rangeHigh"] for partition in setup["partitions"] if partition["name"] == name][0]

    # Get the delay
    delay = [partition["delay"] for partition in setup["partitions"] if partition["name"] == name][0]

    # Print the port
    print("Port: ", port)

    # Create a socket object
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Generate random numbers and send them to the data aggregator
    try:
        while True:
            data = random.randint(low, high)
            s.sendto(str(data).encode(), ("localhost", port))
            print("Sent: ", data)
            print(time.time())
            # Wait for the specified rate
            time.sleep(delay)
    except KeyboardInterrupt:
        # Close the connection on Ctrl+C
        s.close()
        print("Connection closed")

    return


if __name__ == "__main__":
    main()




