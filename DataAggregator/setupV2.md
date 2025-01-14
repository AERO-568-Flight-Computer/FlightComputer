# Info

This document explains how to use the setupV2 file that supports the data aggregator and its communication with individual partitions. 

# setupV2 Doc

Each partition is described by a dictionary. 

For each partition that you want the data aggregator to communicate with, add an entry to the dictionary.

Think of sending and reciving as from the perspective of the partition, the portSend is the port that the partition sends data to the data agregator on and the portRecive is the port that the partition recives data from the data agregator NOT the port that the data agrergator expects to recive data on.

Necessary dictionary entries:
- name: "name"
    unique (to this file) name of partition
    (string)

- portSend: 12345
    unique (to the whole FC) port to send data to aggregator
    (int) or ()

- rate: 1
    avg time between sending data. This allows data agg to check if things are on time.
    (Hz)

- sendDict
    dict where the keys are the index in the numpy array and the values are the name of the data

Optional:

- portReceive: 12355
    unique (to the whole FC) port to receive data from the aggregator
    
- receiveDict
    dict with desired order of data sent back to partition by the data aggregator. If none, no data sent back

