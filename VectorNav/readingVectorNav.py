import serial
import time
import struct

# port on my laptop being used is COM3 and the baudrate for the VN-310 is 115200
port = serial.Serial(port='/dev/ttyUSB0', baudrate=115200)
size = 138 # Size of the message in bytes 

def calculate_crc(data):
    crc = 0x0000
    for byte in data:
        crc = (crc >> 8) | (crc << 8) & 0xFFFF
        crc ^= byte
        crc ^= (crc & 0xFF) >> 4
        crc ^= (crc << 12) & 0xFFFF
        crc ^= ((crc & 0xFF) << 5) & 0xFFFF
    return crc

try:
    while True:

        try:
            
            # Initialize to start reading the message
            byte = port.read(1) # Read the first byte of the message
            starting_byte = b'\xfa' # The starting byte of the message
            list_of_bytes = [] # List to store the bytes of the message

            # Wait for the starting byte of the message
            while byte != starting_byte:
                byte = port.read(1)
            
            # Read the rest of the message
            list_of_bytes.append(starting_byte)
            for i in range(size - 1):
                list_of_bytes.append(port.read(1))

            # Hardcoded indices to extract the payload from the message // Change to account for different message sizes
            payload_list = list_of_bytes[10:136]

            # Parse the payload
            parsed_payload_list = b''
            for byte in payload_list:
                parsed_payload_list += byte

            # Split the parsed payload into payload groups
            TIME = parsed_payload_list[0:8]
            GNSS = parsed_payload_list[8:46]
            ATTITUDE = parsed_payload_list[46:82]
            INS = parsed_payload_list[82:126]

            ''' Calculate the CRC of the message '''
            
            data = b''
            for byte in list_of_bytes[1:138]:
                data += byte
            calculated_crc = calculate_crc(data)

            ## Check if the calculated CRC matches the CRC in the message
            if calculated_crc == 0:
                pass
            else:
                raise ValueError("[WARNING] Message may be corrupted! CRC does not match.")

            ''' Extracting Group Fields from the payload groups '''
            # TIME
            Time_UTC = TIME[0:8]

            # GNSS
            NumSats = GNSS[0:1]
            GnssFix = GNSS[1:2]
            GnssPosLla = GNSS[2:26]
            GnssVelNed = GNSS[26:38]

            printNumSats = f"Numeber of Satellites: {struct.unpack('b', NumSats[0:1])[0]}"
            printGnssFix = f"GNSS Fix: {struct.unpack('b', GnssFix[0:1])[0]}"

            # ATTITUDE
            Ypr = ATTITUDE[0:12]
            LinBodyAccel = ATTITUDE[12:24]
            Ypru = ATTITUDE[24:36]

            # INS
            INSPosLla = INS[0:24]
            INSVelNed = INS[24:36]
            PosU = INS[36:40]
            VelU = INS[40:44]

            printPosU = f"Position Uncertainty: {struct.unpack('f', PosU[0:4])[0]}"
            printVelU = f"Velocity Uncertainty: {struct.unpack('f', VelU[0:4])[0]}"

            ## Outputs of the specific fields

            # Time_UTC
            #year = struct.unpack('b', Time_UTC[0:1])[0]
            #month = struct.unpack('b', Time_UTC[1:2])[0]
            #day = struct.unpack('b', Time_UTC[2:3])[0]
            hour = struct.unpack('b', Time_UTC[3:4])[0]
            minute = struct.unpack('b', Time_UTC[4:5])[0]
            second = struct.unpack('b', Time_UTC[5:6])[0]
            msec = struct.unpack('h', Time_UTC[6:8])[0]
            printTime = f"{hour}:{minute}:{second}.{msec}"

            # GnssPosLla
            Latitude = struct.unpack('d', GnssPosLla[0:8])[0]
            Longitude = struct.unpack('d', GnssPosLla[8:16])[0]
            Altitude = struct.unpack('d', GnssPosLla[16:24])[0]
            printGnssPosLla = f"Latitude: {Latitude}, Longitude: {Longitude}, Altitude: {Altitude}"

            # GnnsVelNed
            vel0 = struct.unpack('f', GnssVelNed[0:4])[0]
            vel1 = struct.unpack('f', GnssVelNed[4:8])[0]
            vel2 = struct.unpack('f', GnssVelNed[8:12])[0]
            printGnssVelNed = f"Velocity: {vel0}, {vel1}, {vel2}"

            # Ypr
            Yaw = struct.unpack('f', Ypr[0:4])[0]
            Pitch = struct.unpack('f', Ypr[4:8])[0]
            Roll = struct.unpack('f', Ypr[8:12])[0]
            printYpr = f"Yaw: {Yaw}, Pitch: {Pitch}, Roll: {Roll}"

            # LinBodyAccel
            accel0 = struct.unpack('f', LinBodyAccel[0:4])[0]
            accel1 = struct.unpack('f', LinBodyAccel[4:8])[0]
            accel2 = struct.unpack('f', LinBodyAccel[8:12])[0]
            printLinBodyAccel = f"Linear Body Acceleration: {accel0}, {accel1}, {accel2}"

            # Ypru
            Yawu = struct.unpack('f', Ypru[0:4])[0]
            Pitchu = struct.unpack('f', Ypru[4:8])[0]
            Rollu = struct.unpack('f', Ypru[8:12])[0]
            printYpru = f"YawU: {Yawu}, PitchU: {Pitchu}, RollU: {Rollu}"

            # INSPosLla
            LatitudeINS = struct.unpack('d', INSPosLla[0:8])[0]
            LongitudeINS = struct.unpack('d', INSPosLla[8:16])[0]
            AltitudeINS = struct.unpack('d', INSPosLla[16:24])[0]
            printINSPosLla = f"Latitude: {LatitudeINS}, Longitude: {LongitudeINS}, Altitude: {AltitudeINS}"

            # INSVelNed
            vel0INS = struct.unpack('f', INSVelNed[0:4])[0]
            vel1INS = struct.unpack('f', INSVelNed[4:8])[0]
            vel2INS = struct.unpack('f', INSVelNed[8:12])[0]
            printINSVelNed = f"Velocity: {vel0INS}, {vel1INS}, {vel2INS})"
            
            ## Create Message
            # 02d is used to print the number with 2 digits and 11.6f is used to print the number with 11 digits and 6 decimal places

            Message = (
            f"Time UTC: {hour:02d}:{minute:02d}:{second:02d}.{msec:03d}, "
            f"Yaw: {Yaw:11.6f}, Pitch: {Pitch:11.6f}, Roll: {Roll:11.6f}, "
            f"Linear Body Acceleration: {accel0:11.6f}, {accel1:11.6f}, {accel2:11.6f}"
            )


        except (ValueError, IndexError) as error:
            print(error)
            continue

        # Print Message
        print(Message)

        time.sleep(0.1)

        #Reset the input buffer to prevent overflow
        port.reset_input_buffer()
# To disconnect from the VN-310, press Ctrl+C in the terminal
except KeyboardInterrupt:
    print("Disconnected from VectorNav")
finally:
    port.close()
