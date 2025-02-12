import zmq
import time
import struct

def main():
    try:
        #All the sockets are created here. message brocker is stable, so it binds, everybody else connects
        context = zmq.Context()

        #Servo 1 
        servo1_pos_rcv_socket = context.socket(zmq.REP)
        servo1_pos_rcv_socket.setsockopt(zmq.LINGER,0)


        time.sleep(0.2)
    except:
        print("Failed to set up inbound connection")
        context.term()
        print("Networking closed")
        raise


    def unpack_servo_cmd(msg_body):
        time_msg_sent, servo_angle_req = struct.unpack('dd',msg_body)
        return time_msg_sent, servo_angle_req
        
    def pack_servo_pos(time_pos_read,servo_pos_deg):
        msg = struct.pack('dd',time_pos_read,servo_pos_deg)

    def unpack_servo_pos(msg_body):
        time_pos_read,servo_pos_deg = struct.unpack('dd',msg_body)
        return time_pos_read, servo_pos_deg

    try:
        test_servo_port = '/dev/ttyS4'
        test_servo_id = 0x01
        TestServo = Servo(test_servo_port, test_servo_id)
        print("Servo instance created")
    except:
        print("Servo creation failed. Closing")
        context.term()
        raise


    time_since_last_send = 0
    time_of_last_send = time.time()
    try:
        while True:
                print("Hi, main servo loop here")
                time.sleep(0.2)
    except KeyboardInterrupt:
        print("Closed from keyboard")
        context.term()
    except:
        print("Some error in the main loop")
        context.term()

if __name__ == '__main__' :
    main()