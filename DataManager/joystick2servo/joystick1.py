import socket
import struct
from NGIcalibration1 import *
from time import sleep, time
# from PartitionManager.partitonManager import initialize

# Calculates force based on speed
def calcForce(airspeed):
    if airspeed < 5:
        airspeed = 5
    return airspeed / 4

# Sends force value to the NGI
def adjustForce(ngi, axis, ias):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule

    if ias < 5:
        ias = 5
    force = calcForce(ias)

    if axis == 'pitch':
        scale = 1.5
    else:
        scale = 1

    # print(f"ias: {ias} | force: {force}")

    ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
    ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
    ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis),
                      (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))

# Recieves data from the NGI and sends it to the Data Manager through UDP.

def interact(ngi, writer=None):
    rollNorm = 0
    pitchNorm = 0
    throttle = 0
    count = 0
    pitchTrimVal = 0
    rollTrimVal = 0
    trimStep = 0.01

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   
    # rxSockStatus = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    next_send_time = time() + 10  # Set the initial time to send data after 10 seconds

    while True:
        
        ias = 20  # Placeholder for IAS

        # Adjust Force Schedule Based on IAS
        if count > 20:
            adjustForce(ngi, 'pitch', ias)
            adjustForce(ngi, 'roll', ias)
            count = 0
        count += 1

        """ RECEIVE FROM PORT 7004"""

        print("Waiting to receive data")
        
        data, addr = ngi.rxSockStatus.recvfrom(4096)
        # client.sendto( 1  , ('localhost', 22222))
        # print(data)
        # Check if it's time to send data to port 11111
        if time() >= next_send_time:
            client.sendto(data, ('localhost', 11111))
            next_send_time = time() + 0  # Update the next sending time

def main():

    ngi = StirlingInceptor()

    try:

        """ IBIT """
        ngi.IBIT()

        """ ACTIVATION """
        ngi.activate()

        """ ADJUST CALIBRATION FORCE OFFSET """
        # sleep(2)
        ngi.configSetup()
        # sleep(2)

        # initialize.initialize() #place this line at a point in your partition where the setup is complete
        time.sleep(0.5) #adds delay to make sure that the server is setup
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #creates a TCP based socket
        client.connect(('localhost', 54321)) #connects socket to the partiton manager as a client
        client.send(b'success') #sends a message that tells the partiton manager that initialization has been completed        
        """ STIRLING INTERACTION """
        interact(ngi)


    except KeyboardInterrupt as e:
        print(e)
    finally:
        print("Data transmission terminated by user.")

if __name__ == '__main__':
    main()
    

