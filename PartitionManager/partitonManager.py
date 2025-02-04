import time
import json
import subprocess
import socket
import psutil
from colorama import Fore, Back, Style

def main():
    # close_all_sockets() #ensures all sockets are closed

    with open("PartitionManager/joystickTest.json") as f: #loads in data
        partitionInfo = json.load(f)

    partitionInfo = sorted(partitionInfo, key=lambda x: x['priority']) #sorts partitions by priority so the most important ones open first

    p = []
    nameList = []

    for partition in partitionInfo:
        p.append(subprocess.Popen(['xterm -e python3 '+partition['path']], shell=True)) #opens partition and appends the Popen object to a list

        nameList.append(partition['name'])       
       
        print(partition['name']+' has been launched') #confirms attempt to launch
        time.sleep(0.1)

    openPrograms = nameList.copy();

    while len(openPrograms) is not 0: #checks to see if all processes are closed
        for item in range(0, len(nameList)): #runs through all the processes
            if p[item].poll() is None: #checks if item is closed
                if nameList[item] not in openPrograms: openPrograms.append(nameList[item]) #add back into open programs if it reopens somehow
                print(Style.RESET_ALL+nameList[item]+' is running')
            else:
                if nameList[item] in openPrograms: openPrograms.remove(nameList[item]) #sees what processes are still open
                print(Fore.YELLOW+nameList[item]+' has closed')
            time.sleep(0.4)

    print(Fore.RED+'ALL PROGRAMS HAVE EXITED')

def close_all_sockets():
    for proc in psutil.process_iter(['pid', 'name', 'username']):
        if proc.info['name'] == 'python':  # Filter for Python processes
            for conn in proc.info['username']:
                if conn.status == 'ESTABLISHED':  # Check if connection is established
                    try:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, conn.fd)
                        sock.close()
                    except OSError:
                        pass  # Ignore if the socket is already closed

if __name__ == "__main__":
    main()