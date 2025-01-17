import socket 
import subprocess
import time

# starts all processes to run joystick servo demo
# does not verify processes are running properly
# is having serial and/or sockets errors for me

def main():
    subprocess.Popen(["python3", "DataManager/joystick2servo/joystick1.py"])
    print('joystick1 opened')
    time.sleep(2)
    subprocess.Popen(["python3", "DataManager/joystick2servo/testDataManager1.py"])
    print('testDataManager1 opened')
    time.sleep(2)
    subprocess.Popen(["python3", "OfficialPartitions/ElevatorServo.py"])
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

