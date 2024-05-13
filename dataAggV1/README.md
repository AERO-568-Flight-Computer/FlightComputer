## Danilo's Data Aggregator

# Description

This data aggregator is designed to follow the ARINC 653 inspired architecture by Carl Denslow. The current version can receive a single integer piece of data per packet from an arbitrary number of partitions. It is multithreaded and has a thread dedicated to listening to each partition. There is another thread that saves the data every so often. 

# How to use

1 Edit configuration file "setup.json". You can set the rate for the example partitions in the setup file.

2 Open a terminal for each partition, these load the data file to figure out which port to talk over. 

3 In each terminal, run the sample partitions "**python3 simplePartition.py partition1 setup.json**". Change partition1 to partition 2 and so on. There is a file that can do this automatically "startPartitionsLinux.sh". I think it will work, but only on linux.

4 In a separate terminal, start the data aggragator. It will make a folder that stores the data. If the files already exist, it will just add the the end of them. ( So probably delete or move the old files)

