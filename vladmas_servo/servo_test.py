import time
#Test for the servo class.
#Going to run the servo through the whole range of motion.
# Positions request : [-50 -25 0 25 50 ]
# 1 second delay betweeen the position.
# if can't set postion, going to say I cant set position
from Servo import Servo
elevator_servo_port = '/dev/ttyS4'
elevator_servo_id = 0x01
ElevatorServo = Servo(elevator_servo_port, elevator_servo_id)

positions = [-50, -25, 0, 25, 50]
delay = 2

while True:
    for pos in positions:
        set_pos_err_code = ElevatorServo.set_pos(pos)
        if set_pos_err_code != 0:
            print("set_pos failed with exit code?:")
            print(set_pos_err_code)
            time.sleep(delay)