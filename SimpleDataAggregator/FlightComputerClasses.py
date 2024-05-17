import socket
import random
from time import sleep
import threading

class Partition:
    def __init__(self, ip="127.0.0.1", port=12345, partition_name=""):
        # The only required argument is the partition_name
        self.UDP_IP = ip
        self.UDP_PORT = port
        self.partition_name = partition_name
        # Set the running status
        self.running = threading.Event()
        self.running.set()
        # Create a UDP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        # Register the partition with the server
        data = f'register:{self.partition_name}'
        self.sock.sendto(data.encode('utf-8'), (self.UDP_IP, self.UDP_PORT))

    def put(self, value):
        data = f'put:{self.partition_name}:{value}'
        self.sock.sendto(data.encode('utf-8'), (self.UDP_IP, self.UDP_PORT))

    def get(self):
        data = f'get:{self.partition_name}'
        self.sock.sendto(data.encode('utf-8'), (self.UDP_IP, self.UDP_PORT))
        return self.sock.recv(1024).decode('utf-8')
    
    def partitions(self):
        data = f'partitions:'
        self.sock.sendto(data.encode('utf-8'), (self.UDP_IP, self.UDP_PORT))
        return self.sock.recv(1024).decode('utf-8')

    def list_all_data(self):
        data = f'list:{self.partition_name}'
        self.sock.sendto(data.encode('utf-8'), (self.UDP_IP, self.UDP_PORT))
        
        full_data = ''
        while True:
            part = self.sock.recv(1024).decode('utf-8')
            full_data += part
            if len(part) < 1024:
                # This is the last part
                break
        print(full_data)
        return full_data
    def stop(self):
        print("Stopping partition:", self.partition_name)
        self.running.clear()
    
class JoystickPitch(Partition):
    def __init__(self, ip="127.0.0.1", port=12345, partition_name=""):
        super().__init__(ip, port, partition_name)
    def run(self):
        while self.running.is_set():
            # Generate a number between 0 and 10 and cast as a string
            value = str(random.uniform(0, 10))
            self.put(value)
            sleep(1)

class JoystickRoll(Partition):
    def __init__(self, ip="127.0.0.1", port=12345, partition_name=""):
        super().__init__(ip, port, partition_name)
    def run(self):
        while self.running.is_set():
            # Generate a number between 0 and 10 and cast as a string
            value = str(random.uniform(0, 10))
            self.put(value)
            sleep(4)

class JoystickYaw(Partition):
    def __init__(self, ip="127.0.0.1", port=12345, partition_name=""):
        super().__init__(ip, port, partition_name)
    def run(self):
        while self.running.is_set():
            # Generate a number between 0 and 10 and cast as a string
            value = str(random.uniform(0, 10))
            self.put(value)
            sleep(2)