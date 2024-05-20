import socket
import os
# Use the influxdb library instead of sqlite3
from influxdb import InfluxDBClient
from datetime import datetime
from time import sleep

# Setup the InfluxDB database
# Create a new InfluxDB client
client = InfluxDBClient(host='localhost', port=8086)
# Name of your database
dbname = 'dataDB'
# Check if the database exists and delete it
if dbname in client.get_list_database():
    client.drop_database(dbname)
# Create a new database
client.create_database(dbname)
# Switch to the new database
client.switch_database('dataDB')

# UDP server details
UDP_IP = "127.0.0.1"
UDP_PORT = 12345
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
# Set the socket to non-blocking so we can handle multiple clients
sock.setblocking(0)
#
# Dictionary of Partitions
# Format: {partition:lastCheckin}
# Create empty dictionary if it doesn't exist
if 'currentPartitions' not in globals():
    globals()['currentPartitions'] = {}
#
# Methods to handle the incoming data: register, put, get
# Register a new partition
def register(partition):
    # Check if the partition key exists
    if partition in currentPartitions:
        print("Error: Partition already exists.")
    else:
        # If not, create a new entry with a placeholder timestamp
        currentPartitions[partition] = datetime.now().isoformat()
        print("Partition registered successfully:",partition)
# Insert data into the database
def put(partition, data):
    timestamp = datetime.now().isoformat()
    #print('From dataServer.put():', timestamp, partition, data)
    json_body = [
        {
            "measurement": "dataTable",
            "tags": {
                "partition": partition
            },
            "time": timestamp,
            "fields": {
                "data": value
            }
        }
    ]    
    client.write_points(json_body)
    
def get(partition):
    #print("Getting latest data for client:", partition)
    # Execute the query
    result = client.query(f'SELECT last("data"), time FROM "dataTable" WHERE "partition" = \'{partition}\'')
    # Get the points from the result
    points = list(result.get_points())
    # The latest value and timestamp are the first point in the list
    latest_value = points[0]['last'] if points else None
    latest_time = points[0]['time'] if points else None
    # The latest value is the first point in the list
    latest_value = points[0]['last'] if points else None
    # Send the latest value and timestamp back to the client
    print('Sending latest value and timestamp for:', partition, latest_value, latest_time)
    sock.sendto((str(latest_value) + ', ' + str(latest_time)).encode(), addr)
#
# Main loop to handle incoming data
try:
    print("Server started at", datetime.now())
    while True:
        try:
            # Get the data and address from the client
            data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
            # Decode the data from bytes to string
            data = data.decode('utf-8')
            # 4 Methods: register, put, get, list
            # register
            if data.split(":")[0] == 'register':
                method, partition = data.split(":") # Register the partition using the method below
                register(partition)
            # put
            elif data.split(":")[0] == 'put': # Put the value into the database
                method, partition, value = data.split(":")
                #print("Put data into the database:",method, partition, value)
                put(partition, value)
                #print("Data added to database")
            # get
            elif data.split(":")[0] == 'get':
                method, partition = data.split(":")
                get(partition)
            # Send all registered partitions
            elif data == 'partitions:':
                sock.sendto(str(currentPartitions).encode(), addr)
        except BlockingIOError:
            # No data available, handle it here
            pass
        except Exception as e:
            print(f"An error occurred in the dataServer: {e}")
except KeyboardInterrupt:
    print("Shutting down server...")
    sock.close()



