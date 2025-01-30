import Servo
import time

def main():
    #Runs test if directly called
    test_servo_port = '/dev/ttyS4'
    test_servo_id = 0x01
    TestServo = Servo.Servo(test_servo_port, test_servo_id)
    time.sleep(2)

    while True:
        command = input("Please input angle: ")
        print("Trying to set postion")
        if -55 < command < 55:
            set_pos_err_code = TestServo.set_pos(command)
            if set_pos_err_code != 0:
                print("set_pos failed with exit code?:")
                print(set_pos_err_code)


if __name__ == "__main__":
    main()