#Improvment of pitotRead.py to send data via zmq. untested yet.  
import serial
import struct
from opa_msg_library import *
import time
import zmq

verbose = True
#Defining servo config. id is used for messsages
#ZMQ is goint to raise an exception if send or recieve is unsucesfull withing socket_timeout.
#servo_max_freq is to not tax CPU to much. Just going to sleep for that much at the end. 
adc_id = b'A1'
socket_timeout = 5000 # in milliseconds

#Each socket is supposed to recieve it's own type of message.
if verbose: print("Setting up sockets")
#Setting up sockets. PULL is type to recieve. PUSH to send.
#LINGER 0 makes it close immidiatly when close is caleed for.
#CONFLATE 1 keeps only the last message in the socket.
#ip's are defined in data_agregator_zmq, all connections are to it.
context = zmq.Context()
a1_cmd_rx_sock = context.socket(zmq.PULL)
a1_cmd_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
a1_cmd_rx_sock.setsockopt(zmq.LINGER, 0)
a1_cmd_rx_sock.setsockopt(zmq.CONFLATE, 1)
a1_cmd_rx_sock.connect('tcp://localhost:5580') 

a1_pos_tx_sock = context.socket(zmq.PUSH)
a1_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
a1_pos_tx_sock.setsockopt(zmq.LINGER, 0)
a1_pos_tx_sock.setsockopt(zmq.CONFLATE,1)
a1_pos_tx_sock.connect('tcp://localhost:5581')

ser = serial.Serial('/dev/ttyS0', 115200, timeout=1)

def crc16_custom(data: bytes) -> int:
    crc = 0x0000  # Initial value for 16-bit CRC
    poly = 0x8005  # Common polynomial for 16-bit CRC (x^16 + x^15 + x^2 + 1)

    for byte in data:  
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:  # Check the leftmost bit
                crc = ((crc << 1) & 0xFFFF) ^ poly  # Ensure it stays within 16 bits
            else:
                crc = (crc << 1) & 0xFFFF  # Ensure it stays within 16 bits

    return crc

while True:
    pitot = ser.read(1)
    sync_byte = 255
    if pitot == bytes([sync_byte]):

        print("Received sync byte:", sync_byte)
        byteArray = ser.read(38)

        # Time in miliseconds (4 byte int)
        militime = int.from_bytes(byteArray[0:4], byteorder='little')
        print("Time [ms]: ", militime)

        # Abs Pressure in Pascals (4 byte float)
        absPressure = struct.unpack('f', byteArray[4:8])[0]
        print("Abs Pres [Pa]: ", absPressure)

        # Abs Temperature in Celcius (4 byte float)
        absSenseTemp = struct.unpack('f', byteArray[8:12])[0]
        print("Abs Temp [C]: ", absSenseTemp)

        # Pressure difference in Pascals (4 byte float)
        diffPressureMS = struct.unpack('f', byteArray[12:16])[0]
        print("deltaPres [Pa]: ", diffPressureMS)

        # Temperature difference in Celcius (4 byte float)
        diffSenseTempMS = struct.unpack('f', byteArray[16:20])[0]
        print("deltaTemp [C]: ", diffSenseTempMS)

        # Pressure difference in Pascals (4 byte float)
        diffPressureDL = struct.unpack('f', byteArray[20:24])[0]
        print("deltaPres [Pa]: ", diffPressureDL)

        # Temperature difference in Celcius (4 byte float) #What does dl mean?
        diffSenseTempDL = struct.unpack('f', byteArray[24:28])[0]
        print("deltaTemp [C]: ", diffSenseTempDL)

        # AOA from rear pitot flag
        rearFlagAOA = struct.unpack('f', byteArray[28:32])[0]
        print("AOA [degs]: ", rearFlagAOA)

        # Yaw from front pitot flag
        frontFlagYaw = struct.unpack('f', byteArray[32:36])[0]
        print("Yaw [degs]: ", frontFlagYaw)

        crcArray = bytes([255]) + byteArray[:-2]
        crc_calculated = crc16_custom(crcArray)
        print(crc_calculated)
        # CRC check
        crcCheck = int.from_bytes(byteArray[36:38], byteorder='little')
        print(crcCheck)
        #I would like to pack a message, bit I want it to be very clear
        #What variable goes into what position
        dataDictionary = {
             "militime": militime,
             "absPressure": absPressure,
             "absSenseTemp": absSenseTemp,
             "diffPressureDL": diffPressureDL,
             "diffSenseTempDL": diffSenseTempDL,
             "rearFlagAOA": rearFlagAOA,
             "frontFlagYaw": frontFlagYaw
         }
        
        msg = pack_adc_state_msg(b'P1', time.time(), dataDictionary)

        a1_pos_tx_sock.send(msg)
        time1 = time.time()
        print(f"{time1} : ADC message out: {unpack_servo_pos_msg(msg)}")