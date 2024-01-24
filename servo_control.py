import time
import serial
import volz_actuator as va

def get_user_input():
    degree = int(input("Enter the degree to move the servo (-90 to 90): "))
    increment = int(input("Enter the increment size for servo movement: "))
    return degree, increment

if __name__ == '__main__':
    # set up serial connection
    ser = serial.Serial('/dev/ttyS6', 115200, timeout=1)
    time.sleep(2)  # Wait for the device to initialize


    while True:
        degree, increment = get_user_input()

        if -90 <= degree <= 90:
            cmd = va.build_pos_command(degree)
            ser.write(bytearray(cmd))
            time.sleep(1)  # wait for the servo to move

            degree += increment
            if degree > 90: degree = 90
            if degree < -90: degree = -90
        else:
            print("Degree out of range. Please enter a value between -90 and 90.")
