import serial
import struct

ser = serial.Serial('/dev/cu.usbserial-A9087BP2', 115200, timeout=1)

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
        byteArray = ser.read(30)

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
        diffPressure = struct.unpack('f', byteArray[12:16])[0]
        print("deltaPres [Pa]: ", diffPressure)

        # Temperature difference in Celcius (4 byte float)
        diffSenseTemp = struct.unpack('f', byteArray[16:20])[0]
        print("deltaTemp [C]: ", diffSenseTemp)

        # AOA from rear pitot flag
        rearFlagAOA = struct.unpack('f', byteArray[20:24])[0]
        print("AOA [degs]: ", rearFlagAOA)

        # Yaw from front pitot flag
        frontFlagYaw = struct.unpack('f', byteArray[24:28])[0]
        print("Yaw [degs]: ", frontFlagYaw)

        crcArray = bytes([255]) + byteArray[:-2]
        crc_calculated = crc16_custom(crcArray)
        print(crc_calculated)
        # CRC check
        crcCheck = int.from_bytes(byteArray[28:30], byteorder='little')
        print(crcCheck)

        # dataDictionary = {
        #     "sysTimeStamp": sysTimeStamp,
        #     "militime": militime,
        #     "absPressure": absPressure,
        #     "absSenseTemp": absSenseTemp,
        #     "diffPressure": diffPressure,
        #     "diffSenseTemp": diffSenseTemp,
        #     "rearFlagAOA": rearFlagAOA,
        #     "frontFlagYaw": frontFlagYaw
        # }