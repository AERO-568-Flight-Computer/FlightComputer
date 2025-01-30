import socket
import subprocess
import time
import platform

# starts all processes to run joystick servo demo
# does not verify processes are running properly
# is having serial and/or sockets errors for me
 
import psutil


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


def main(): 
    close_all_sockets() 
    
    new_window_command = "xterm -e"

    subprocess.Popen([new_window_command +" python3 "+"DataManager/joystick2servo/joystick1.py"], shell=True)
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

    
if __name__ == '__main__':
    main()

pass 