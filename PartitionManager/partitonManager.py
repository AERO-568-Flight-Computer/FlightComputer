import time
import json
import subprocess
import socket
import psutil
from colorama import Fore, Back, Style

def main():
    close_all_sockets() #ensures all sockets are closed (does not work on MacOs)

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based server
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('localhost', 54321)) #puts the socket on a local port

    with open("PartitionManager/joystickTest.json") as f: #loads in data
        partitionInfo = json.load(f)

    partitionInfo = sorted(partitionInfo, key=lambda x: x['priority']) #sorts partitions by priority so the most important ones open first

    p = []
    nameList = []

    for partition in partitionInfo: #open all partitions
        p.append(subprocess.Popen(['xterm -T "'+partition['name']+'" -e python3 '+partition['path']], shell=True)) #opens partition and appends the Popen object to a list

        nameList.append(partition['name'])       
       
        print(Style.RESET_ALL+partition['name']+' has been launched, waiting for initialization') #confirms attempt to launch

        checkInitialized(server, partition)

    openPrograms = nameList.copy()

    while len(openPrograms) != 0: #checks to see if all processes are closed
        for item in range(0, len(nameList)): #runs through all the processes
            if p[item].poll() == None: #checks if item is closed
                if nameList[item] not in openPrograms: openPrograms.append(nameList[item]) #add back into open programs if it reopens somehow
                print(Style.RESET_ALL+nameList[item]+' is running')
            else:
                if nameList[item] in openPrograms: openPrograms.remove(nameList[item]) #sees what processes are still open
                print(Fore.YELLOW+nameList[item]+' has closed')

                if partitionInfo[item]['restart'] == "True":
                    print(Style.RESET_ALL+'Attempting restart of '+nameList[item])

                    subprocess.Popen(['xterm -T "'+nameList[item]+'" -e python3 '+partitionInfo[item]['path']], shell=True)

                    print(Style.RESET_ALL+nameList[item]+' has been relaunched, waiting for initialization')

                    checkInitialized(server, partitionInfo[item])
                else:
                    print(Style.RESET_ALL+'Will not restart '+nameList[item])


    print(Fore.RED+'ALL PROGRAMS HAVE EXITED')

def checkInitialized(server, partition):
    server.listen() #tells the server to wait for a client to connect

    client, address = server.accept() #waits for it to connect and creates objects for client and address
        
    data = client.recv(1024) #tells server to expect a certian size packet and to assign that to data
    client.close() #closes the socket for the client, keeps it open for the server so someone else could connect

    if data.decode('utf8') == 'success':
        print(Fore.GREEN+partition['name']+' has been initialized') #confirms attempt to launch
    else:
        print(Fore.RED+partition['name']+' has failed initialization, please retry starting partitons') #confirms attempt to launch
        time.sleep(10)

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