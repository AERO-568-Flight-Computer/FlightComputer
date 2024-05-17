# Simple implementation of a Data Aggregation Server and Partitions
The server will receive UDP packets from partitions (sensors and actuators) and store them in a database.

There are 3 files:
- dataServer.py: implements the InfluxDB and stores and retrieves data for the clients (partitions)
- FlightComputerClasses.py: defines classes used in the runPartitions.py file
- runPartitions.py: Start and monitor the partition classes

## How to run the demo
Open 2 terminal windows, side by side
- On the left terminal, run: python SimpleDataAggregator/dataServer.py
- On the right terminal, run: python SimpleDataAggregator/runPartitons.py
- You can interact by selecting a command in the runPartitions program:
   - Enter command (threads, partitions, get, exit): 

## Implementation
Each partition is implemented as subclasses of a Partition class. To create a new partition, you must:
- Open FlightComputerClasses.py
- Select a name for your new class, e.g. JoystickYaw, VectorNavLongitude, AileronServoPosition, etc.
- Create a new subclass of the Partition Class (see examples in FlightComputerClasses.py)
- Implement a run() method that does the work of your partition.
 - For example, collect data from your sensor, then send the data to the self.put() method (which is defined in the Partition Class)

## Format of the data
This is intended to be a very atomic data server. In other words, the data should be formatted as simple as possible before sending to the server. For example, if there are 3 axes of the joystick, this should be broken into 3 separate parameters and classes: JoystickPitch, JoystickRoll, JoystickYaw.

At this time, it seems like the best way is to format the data value as a sting. We may decide otherwise later on, but it seems like this will work in the current implementation.

All you have to do is define a 'partition_name' and the value as a stirng. For example it could be "3", "3.05234", "-3.2".

The data is sent via UDP sockets to the dataServer. The dataServer recieves it and inserts it into the database.

You don't have to worry about this format, but here is how the dataServer formats and inserts the data.

```
#========================================
# How to add a data point to the database
#
# Create a json object
 json_body = [
     {
         "measurement": "aircraftData",
         "tags": {
             "partition": "partition_name_here"
         },
         "time": datetime.now().isoformat(),
         "fields": {
             "data": "your_data_here"
         }
     }
 ]

# Insert/write the measurement to the database
client.write_points(json_body)
#========================================
```

## Why InfluxDB?

The InfluxDB is a special type of SQL server made for very fast logging of data from many clients. Recent entries exist in memory, but are eventually written to disk for persistence.

This is a very robust and fast way to store data. All the advantages of a relational database without the computational overhead and latency of something like MySQL or similar.

https://www.influxdata.com

How to install InfluxDB on Linux:
- wget https://dl.influxdata.com/influxdb/releases/influxdb_1.8.4_amd64.deb
- sudo dpkg -i influxdb_1.8.4_amd64.deb
- sudo systemctl start influxdb
- sudo systemctl enable influxdb
- sudo systemctl status influxdb

Now you can use the influxdb package in Python. This is implemented for you in the dataServer.py file.