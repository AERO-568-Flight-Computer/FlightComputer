import time
import json
import subprocess

def main():
    with open("setupTest.json") as f:
        partitionInfo = json.load(f)

    print(partitionInfo)


if __name__ == "__main__":
    main()