import socket
from ServoUtilMethods import *
import serial
import struct

# Initialize serial connection to the actuator
ser = serial.Serial('/dev/ttyS6', 115200, timeout=1)

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Set the socket option to allow reusing the address
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

'''
# Close any existing socket on port 12345
try:
    existing_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    existing_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    existing_sock.connect(('localhost', 12345))
    existing_sock.close()
    print("Closed existing socket on port 12345")
except OSError:
    pass
'''

# Bind the socket to the specified port
server_address = ('localhost', 12300)
sock.bind(server_address)

startup = 0
count = 0

running = True
while running:
    data, address = sock.recvfrom(4096)

    # determine what the clutch status is - powered on or off?
    if startup == 0:
        try:
            pwr_clutch = get_pwr_status(ser)[1]
        except:
            print("Error: Could not get clutch status... Servo may not be turned on.")
            continue

    while startup == 0 and pwr_clutch < 20:
        try:
            joystick_position = struct.unpack('f', data)[0]
            servo_current_pos_deg = get_pos(ser)[0]
            print("Servo Current position:", servo_current_pos_deg)
            pwr_clutch = get_pwr_status(ser)[1]
            if pwr_clutch > 20:
                zero_position = servo_current_pos_deg + joystick_position
                print("Setting zero position:", zero_position)
                startCommand = build_pos_command(zero_position)
                ser.write(bytearray(startCommand))
                rx = ser.read(12)
                startup = 1
            else:
                print("Waiting for clutch to be powered on")
        except:
            print("Error: Could not get clutch status... Servo may not be turned on.")
            continue
        
    try:

        joystick_position_zeroed = struct.unpack('f', data)[0] + zero_position
        servo_current_pos_deg = get_pos(ser)[0]
        print("Servo Current position:", servo_current_pos_deg)
        
        if servo_current_pos_deg > -55 and servo_current_pos_deg < 55:
            command = build_pos_command(joystick_position_zeroed)
            ser.write(bytearray(command))
            rx = ser.read(12)
        else:
            print("Servo is at its limit. Cannot move further.")
            continue

        if count % 2 == 0:
            pwr_clutch = get_pwr_status(ser)[1]
            if pwr_clutch == 0:
                print("Clutch is not powered on")
                startup = 0

        count += 1

    except:
        print("Error: Could not get clutch status... Servo may not be turned on.")
        continue

# Close socket
sock.close()

