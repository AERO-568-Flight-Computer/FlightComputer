import os 
import socket 

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind socket to specified port
    server_address = ('localhost', 22222)
    sock.bind(server_address)
    
    os.system("joystick1.py") #may require file path

    data, address = sock.recvfrom(4096) #TODO verify that less buffer can be allocated

    if data == 1:

        os.system("testdatamanager1.py") #may require file path

        #TODO check if testdatamanager1.py is running correctly 

        os.system("elevatorservo.py") #may require file path

        #TODO check if elevatorservo.py is running correctly
    else:
        print("Error Starting joystick1.py")









if __name__ == '__main__':
    main()

