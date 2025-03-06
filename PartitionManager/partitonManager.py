import time
import json
import subprocess
import socket
import psutil
from colorama import Fore, Back, Style
from tkinter import messagebox as mb
import sys

port = 54321

class initialize:
    port = 54321
    def initialize():
        import socket
        import time

        time.sleep(0.5) #adds delay to make sure that the server is setup
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based socket
        client.connect(('localhost', 54321)) #connects socket to the partiton manager as a client
        client.send(b'success') #sends a message that tells the partiton manager that initialization has been completed
        print('Initialization signal sent')

def main():
    #close_all_sockets() #ensures all sockets are closed (does not work on MacOs)

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based server
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('localhost', port)) #puts the socket on a local port

    with open("Configurations/joystickTest_zmq.json") as f: #loads in data
        partitionInfo = json.load(f)

    partitionInfo = sorted(partitionInfo, key=lambda x: x['priority']) #sorts partitions by priority so the most important ones open first

    p = [] #initlalized a list that stores objects that are connected to each subprocess
    nameList = [] #creates a list that makes it easy to call the name of a partiton

    for partition in partitionInfo: #open all partitions
        p.append(subprocess.Popen(['xterm -T "'+partition['name']+'" -e python3 '+partition['path']], shell=True)) #opens partition and appends the Popen object to list p
        nameList.append(partition['name']) #adds name to namelist
        # time.sleep(20)
        print(Style.RESET_ALL+partition['name']+' has been launched, waiting for initialization') #confirms attempt to launch
        checkInitialized(server, partition) #ensures partitions have been initilazied

    openPartitons = nameList.copy() #creates list that stores a list of whatever partitons are open

    while len(openPartitons) != 0: #checks to see if all partitons are closed
        for item in range(0, len(nameList)): #runs through all the partitions
            if p[item].poll() == None: #checks if is closed
                if nameList[item] not in openPartitons: openPartitons.append(nameList[item]) #add back into list if it reopens
                print(Style.RESET_ALL+nameList[item]+' is running')
            else: #runs if program is closed
                print(Fore.YELLOW+nameList[item]+' has closed')
                if nameList[item] in openPartitons: openPartitons.remove(nameList[item]) #notes that partiton is closed
                try:
                    if partitionInfo[item]['restart'].lower() == "true": #runs if program asked to restart
                        print(Style.RESET_ALL+'Attempting restart of '+nameList[item])
                        p[item] = subprocess.Popen(['xterm -T "'+partition['name']+'" -e python3 '+partition['path']], shell=True)
                        # print(Style.RESET_ALL+nameList[item]+' has been relaunched, waiting for initialization')
                        # checkInitialized(server, partitionInfo[item])

                    elif partitionInfo[item]['restart'].lower() == "ask": #runs if program asked if it should restart
                        print(Style.RESET_ALL+'Seeing if restart of '+nameList[item]+' is requested')
                        option = mb.askyesno(title='Restart', message='Would you like to restart '+nameList[item]+'?') #creates dialog box

                        if option == 1: #if answered yes, attempts restart
                            print(Style.RESET_ALL+'Attempting restart of '+nameList[item])
                            p[item] = subprocess.Popen(['xterm -T "'+partition['name']+'" -e python3 '+partition['path']], shell=True)
                            # print(Style.RESET_ALL+nameList[item]+' has been relaunched, waiting for initialization')
                            # checkInitialized(server, partitionInfo[item])

                        else:
                            print(Style.RESET_ALL+nameList[item]+' not restarted per instructions')
                            partitionInfo[item]['restart'] = "false" #does not ask to restart again

                except:
                    print(Style.RESET_ALL+nameList[item]+' not restarted')

    server.close()
    print(Fore.RED+'ALL PROGRAMS HAVE EXITED')
    sys.exit(0)

def checkInitialized(server, partition):
    server.listen() #tells the server to wait for a client to connect
    client, address = server.accept() #waits for it to connect and creates objects for client and address
    data = client.recv(1024) #tells server to expect a certian size packet and to assign that to data
    client.close() #closes the socket for the client, keeps it open for the server so someone else could connect

    if data.decode('utf8') == 'success':
        print(Fore.GREEN+partition['name']+' has been initialized') #confirms attempt to launch
    else:
        print(Fore.RED+partition['name']+' has failed initialization, exiting program') #confirms attempt to launch
        sys.exit(0)

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