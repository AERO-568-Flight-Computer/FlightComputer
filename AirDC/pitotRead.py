import serial
import struct
import numpy as np
import binascii

ser = serial.Serial('/dev/cu.usbserial-A9087BP2', 115200, timeout=1)

while True:
    pitot = ser.read(1)
    # print(pitot)
    sync_byte = 255
    bytelist = []
    byteArray = b''
    if pitot == bytes([sync_byte]):
        print("Received sync byte:", sync_byte)
        bytelist.append(pitot)
        for i in range(0, 32):
            pitot = ser.read(1)
            bytelist.append(pitot)
        for byte in bytelist:
            byteArray += byte

        # Time in miliseconds (4 byte int)
        militime = int.from_bytes(byteArray[1:5], byteorder='little')

        # Abs Pressure in Pascals (4 byte float)
        absPressure = struct.unpack('f', byteArray[5:9])[0]

        # Abs Temperature in Celcius (4 byte float)
        absSenseTemp = struct.unpack('f', byteArray[9:13])[0]

        # Pressure difference in Pascals (4 byte float)
        diffPressure = struct.unpack('f', byteArray[13:17])[0]

        # Temperature difference in Celcius (4 byte float)
        diffSenseTemp = struct.unpack('f', byteArray[17:21])[0]

        # AOA from rear pitot flag
        rearFlagAOA = struct.unpack('f', byteArray[21:25])[0]

        # Yaw from front pitot flag
        frontFlagYaw = struct.unpack('f', byteArray[25:29])[0]

        # Create the dataDictionary
        dataDictionary = {
            "militime": militime,
            "absPressure": absPressure,
            "absSenseTemp": absSenseTemp,
            "diffPressure": diffPressure,
            "diffSenseTemp": diffSenseTemp,
            "rearFlagAOA": rearFlagAOA,
            "frontFlagYaw": frontFlagYaw
        }

        dataDictionaryList = [dataDictionary]

        # CRC check
        crcCheck1 = int.from_bytes(byteArray[29:31], byteorder='little')
        print(crcCheck1)
        # CRC match confirmation
        crcCheck2 = int.from_bytes(byteArray[31:33], byteorder='little')
        print(crcCheck2)
