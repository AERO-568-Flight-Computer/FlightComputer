## partitonManager.py
Written by Ajay Parikh

## Objective
Start and ensure running of all partitons

## Order of operations
Read .json file (eventually sync with DataAggregator) with the partitons you want to launch and some additonal relevant information

Close all open ports

Start partiton in a new terminal window
Ensure that said partiton has initilzed, see example client code in clientExample.py
Repeat last 2 steps until all partitons are open 

Checks that partitons are still running
If specifed in .json file, restarts partiton if it is no longer running
Repeat last 2 steps until program ended

## Debugging help

If you have an error that Address already in use for the socket after trying to restart the progarm, type "fg" then "ctrl+c". Then, try running the program again.