import socket
import subprocess
import time
import platform

# starts all processes to run joystick servo demo
# does not verify processes are running properly
# is having serial and/or sockets errors for me

def main():
    #if platform.system() == "Windows":
    #    new_window_command = "cmd.exe /c start"
    #elif platform.system() == "Darwin": 
    #   new_window_command = "ttab" #you need to install ttab
    #else:  #XXX this can be made more portable
    new_window_command = " " #"gnome-terminal -e"

    # print((new_window_command+" python3 "+"DataManager/joystick2servo/testDataManager1.py"))

    # subprocess.Popen([new_window_command+" python3 "+"DataManager/joystick2servo/testDataManager1.py"], shell=True)

    subprocess.Popen([new_window_command +" DataManager/joystick2servo/joystick1.py"], shell=True)
    time.sleep(1)
    print('joystick1 opened')
    time.sleep(1)
    subprocess.Popen([new_window_command+" python3 "+"DataManager/joystick2servo/testDataManager1.py"], shell=True)
    time.sleep(1)
    print('testDataManager1 opened')
    time.sleep(1)
    subprocess.Popen([new_window_command+" python3 "+"OfficialPartitions/ElevatorServo.py"], shell=True)
    time.sleep(1)
    print('ElevatorServo opened')

# def main():

#     sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#     sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

#     # Bind socket to specified port
#     server_address = ('localhost', 22222)
#     sock.bind(server_address)

#     os.system("DataManager\joystick2servo\joystick1.py") #may require file path

#     data, address = sock.recvfrom(4096) #TODO verify that less buffer can be allocated

#     if data == 1:
#         print("joystick1.py started")
#         os.system("DataManager\joystick2servo\testDataManager1.py") #may require file path

#         #TODO check if testdatamanager1.py is running correctly 

#         os.system("OfficialPartitions\ElevatorServo.py") #may require file path

#         #TODO check if elevatorservo.py is running correctly
#     else:
#         print("Error Starting joystick1.py")









if __name__ == '__main__':
    main()

pass 