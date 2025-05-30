#!/bin/bash

# Change directory
cd /home/cp-opa/Desktop/FlightComputer

# Activate and enter the Virtual Environment
source .venv/bin/activate
echo "Entered the virtual environment..."

# Run the Partition Manager
python3 log_parse.py
echo " "
echo "Log Parsed. Please exit, will automatially exit in one hour."

sleep 1h