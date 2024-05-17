## simpleDataAgg
Created by Danilo Carrasco

# Description

This data aggregator is designed to follow the ARINC 653 inspired architecture by Carl Denslow. It is not fully in compliance with this spec yet, as the scheduler is yet to be implemented.

For the new (under construction) version:

# What is received:
This is capable of receiving numpy 2D 64 bit float arrays from partitions. The columns in these arrays will represent the different types of data. Rows represent the time, with the highest row index being the most recent time.

# What is sent:
The aggregator sends data to the partition at the same rate the partition sends data to it. This packet will contain a numpy array for each partition from which a given partition is requesting data. It will only send the fields that are requested from the partitions, in the order which they are requested. If the requested data doesn't exist yet, the packet will be padded with a nan array to be the same amount of columns requested. 

# How is data stored:
The data aggregator maintains a CVT for each partition (2D numpy 64bit float array). It also retains a time table that records when each data point was obtained (2D numpy 64bit float array). The aggregator also intermittently stores all the data in csv files. These are updated at the rate the user specifies. These are for archival/data acquisition/debugging purposes, they are not referenced by any FC software. A folder "DataAggRecords" will be created in the current directory. If there are already save files there, more data will likely be added to the end of then, not overwritten. Please delete or move the files each time you start the data aggregator, so your data is not tampered with.

# How to use

1 Edit configuration file "setupV2.json". This is how you tell the data aggregator which partitions to listen to. Also, this is where you specify the ports to send and receive data over. Please refer to setupV2.md for more info about the setup file.

2 Start the data aggregator or the partitions. 


Data aggregator will only exit cleanly if the partitions are closed after the aggregator is closed.



# TODO:

1 Figure out how to exit out of the threads cleanly on keyboard interrupt. This might mean keeping a list of threads and having the main thread handle that.

2 Looks like I will need an event to signal all the other threads to stop.