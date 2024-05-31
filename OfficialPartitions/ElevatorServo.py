import socket
from ServoUtilMethods import *
import serial
import struct
import time

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

# Function to update trim
delayInterval = 1
lastTrim_elv = time.time()
trimSum_elv = 0 # degrees

def updateTrim_elv(trimup, trimdwn):
    global trimSum_elv, lastTrim_elv
    current = time.time()
    if current - lastTrim_elv >= delayInterval:
        if trimup == 1:
            trimSum_elv += 1
        if trimdwn == 1:
            trimSum_elv -= 1
    return trimSum_elv

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
            joystick_position, trimup, trimdwn = 0, 0, 0
            trimSum_elv = 0
            servo_current_pos_deg = get_pos(ser)[0]
            print("Servo Current position:", servo_current_pos_deg)
            pwr_clutch = get_pwr_status(ser)[1]
            if pwr_clutch > 20:
                zero_position = servo_current_pos_deg + joystick_position
                if zero_position < -55:
                    zero_position = -55
                elif zero_position > 55:
                    zero_position = 55
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

        joystick_position, trimup, trimdwn = struct.unpack('fff', data)
        check = joystick_position + trimSum_elv
        if check < -55:
            trimSum_elv = updateTrim_elv(trimup, 0)
        elif check > 55:
            trimSum_elv = updateTrim_elv(0, trimdwn)
        else :
            trimSum_elv = updateTrim_elv(trimup, trimdwn)
        joystick_position_zeroed = joystick_position + zero_position + trimSum_elv
        servo_current_pos_deg = get_pos(ser)[0]
        print("Servo Current position:", servo_current_pos_deg)
        
        if -55 < joystick_position_zeroed < 55:
            command = build_pos_command(joystick_position_zeroed)
            ser.write(bytearray(command))
            rx = ser.read(12)
        else:
            print("Joystick position out of range")

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

