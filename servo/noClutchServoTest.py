import NoClutchServo
import time

def main():
    test_servo_port = '/dev/ttyS4'
    test_servo_id = 0x02
    TestServo = NoClutchServo.Servo(test_servo_port, test_servo_id) #calls servo class and creates servo
    time.sleep(2)

    while True:
        command = float(input("Please input angle: "))
        if -55 < command < 55: #make sure capstans are not hit
            print("Trying to set postion")
            set_pos_err_code = TestServo.set_pos(command) #send servo to angle
            if set_pos_err_code != 0: #catch any errors
                print("set_pos failed with exit code?:")
                print(set_pos_err_code)
        else:
            print("Angle out of range, input angle between -55° and 55°")


if __name__ == "__main__":
    main()