import socket
from ServoUtilMethods import *
import serial
import struct
import time
from PartitionManager.partitonManager import initialize

# Initialize serial connection to the actuator
ser = serial.Serial('/dev/ttyS4', 115200, timeout=1)

time.sleep(2)

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

def updateTrim_elv(trimup, trimdwn):
    trim=0
    if trimup == 1:
        trim = 1
    elif trimdwn == 1:
        trim = -1
    return trim
	
startup = 0
currentTrim = 0

# Angle limits to keep the servo within (so it doesn't hit capstan limits)
angleLimMin = -55
angleLimMax = 55

initialize.initialize()

running = True
while running:
    data, address = sock.recvfrom(4096)

    # determine what the clutch status is - powered on or off?
    if startup == 0:
        try:
            pwr_clutch = get_pwr_status(ser)[1]
        except:
            print("Error: Could not get clutch status... Servo may not be turned on.1")
            continue

    while startup == 0:
        try:
            joystick_position, trimup, trimdwn = struct.unpack('fff', data)
            servo_current_pos_deg = get_pos(ser)[0]
            print("Servo Current position:", servo_current_pos_deg)
            pwr_clutch = get_pwr_status(ser)[1]
            if pwr_clutch > 20:
                zero_position = servo_current_pos_deg + joystick_position
                if zero_position < angleLimMin:
                    zero_position = angleLimMin
                elif zero_position > angleLimMax:
                    zero_position = angleLimMax
                startCommand = build_pos_command(zero_position)
                ser.write(bytearray(startCommand))
                rx = ser.read(12)
                startup = 1
                currentTrim = 0
            else:
                print("Waiting for clutch to be powered on")
                print(pwr_clutch) 
        except:
            print("Error: Could not get clutch status... Servo may not be turned on.2")
            continue
        
    try:

        joystick_position, trimup, trimdwn = struct.unpack('fff', data)
        print("Joystick position: ", joystick_position)
        print("Zero position: ", zero_position)
        print("Trim up: ", trimup)
        print("Trim down: ", trimdwn)
        
        # Don't add trim if already at limit
        if (joystick_position + zero_position + currentTrim) > angleLimMax and updateTrim_elv(trimup, trimdwn) == 1:
            currentTrim = currentTrim
        else:
            currentTrim += updateTrim_elv(trimup, trimdwn)

        if (joystick_position + zero_position + currentTrim) < angleLimMin and updateTrim_elv(trimup, trimdwn) == -1:
            currentTrim = currentTrim
        else:
            currentTrim += updateTrim_elv(trimup, trimdwn)

        joystick_position_zeroed = joystick_position + zero_position + currentTrim

        servo_current_pos_deg = get_pos(ser)[0]
        print("Joystick position zeroed: ", joystick_position_zeroed)
        print("Servo Current position:", servo_current_pos_deg)
        print("Startup", startup)
        
        if angleLimMin < joystick_position_zeroed < angleLimMax:
            command = build_pos_command(joystick_position_zeroed)
            ser.write(bytearray(command))
            rx = ser.read(12)
        else:
            print("Joystick position out of range")

        if pwr_clutch == 0:
            print("Clutch is not powered on")
            startup = 0

    except:
        print("Error: Could not get clutch status... Servo may not be turned on.3")
        continue

# Close socket
sock.close()

