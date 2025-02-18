from servo_module import Servo
import time
import socket
import serial
import struct
from DataProcessor import DataProcessor

test_servo_port = '/dev/ttyS4'
test_servo_id = 0x01
TestServo = Servo.Servo(test_servo_port, test_servo_id) # Calls the Servo class and creates servo
time.sleep(2)

name = "servo_demo"
filepath = "./joystick_servo.json"

# Create an instance of the DataProcessor class specified by the name and filepath
processor = DataProcessor(name, filepath)

# Display the attributes of the processor
print(processor.name)
print(processor.portSend)
print(processor.portReceive)

dataDictionaryList = [
    {
        "timeRec": None,
        "position": None,
        "clutchStatus": None
    }
]

while True:
    int_time = time.time()
    
    
    pos,_ = TestServo.get_pos()
    #print(pos)
    _, clutch = TestServo._get_pwr_status()
    
    #print(clutch)
    processor.receiveData()

    recentData = processor.getRecentData("fc_demo", 1)
    timeRecReceived = recentData[0, 0]
    AngleCommandReceived = recentData[0, 1]
    dataDictionaryList[0]["timeRec"] = int_time
    dataDictionaryList[0]["position"]  = pos
    #dataDictionaryList[0]["clutchStatus"] = clutch
    print(dataDictionaryList)
    processor.sendData(dataDictionaryList)

    if -55 < AngleCommandReceived < 55: # Make sure the capstans are not hit
        print("Trying to set postion")
        set_pos_err_code = TestServo.set_pos(AngleCommandReceived) # Send the servo to the given angle
        if set_pos_err_code != 0: # Catch any errors
            print("set_pos failed with exit code?:")
            print(set_pos_err_code)
    else:
        print("Angle out of range, input angle between -55° and 55°")