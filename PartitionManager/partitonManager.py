import time
import json
import subprocess
import socket
import psutil

def main():
    # close_all_sockets() #ensures all sockets are closed

    with open("PartitionManager/setupTest.json") as f: #loads in data
        partitionInfo = json.load(f)

    partitionInfo = sorted(partitionInfo, key=lambda x: x['priority']) #sorts partitions by priority so the most important ones open first

    for partition in partitionInfo:
        subprocess.Popen(['xterm -e python3 '+partition['path']], shell=True) #opens partition
        print(partition['name']+' has been launched')
        time.sleep(0.1)


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