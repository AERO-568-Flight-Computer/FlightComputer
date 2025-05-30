#!/bin/bash

# Activate and enter the Virtual Environment
source .venv/bin/activate
echo "Entered the virtual environment..."

# Run the Partition Manager
python3 PartitionManager/partitonManager.py
echo "Launched the partition manager..."