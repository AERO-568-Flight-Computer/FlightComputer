from opa_msg_library import *

import zmq
import time
import struct
from DummyServo import Servo

def main():
    try:
        context = zmq.Context()
        #I'd need a file per servo, the idea is, we hardcode the topology, since, we do not need
        #to handle situation when some unknown nodes connect, the dissapear, so on.
        #We have a network, that is not limited by throuput, and all the nodes are known.
        #So, star topology.
        
        #Change socket ports MANUALY. and keep track of them, we don't have many node.
        #So we'll manage

        #Servo1
        servo1_id = b'S1'
        send_sock_ip = "tcp://localhost:5556"
        rcv_sock_ip = "tcp://localhost:5557"
        hb_sock_ip = "tcp://localhost:5558"

        # This one is to send position to the message brocker
        sock_pos_send = context.socket(zmq.PUSH) # This one is to send position to the message brocker
        sock_pos_send.setsockopt(zmq.LINGER,0) #LINGER 0 makes it close immideatly when close is called, SNDHWM 40 keeps only 40 messages before it starts dropping them.
        sock_pos_send.setsockopt(zmq.SNDHWM,40)
        sock_pos_send.setsockopt(zmq.SNDTIMEO,4000)
        #40 is probably enough, since I am planning to send servo pos only about 100 per second.
        #If HWM is reached, alarm.
        sock_pos_send.connect(send_sock_ip)

        # This one is to receive pos commands from the message brocker
        sock_cmd_rcv  = context.socket(zmq.PULL)
        sock_cmd_rcv.setsockopt(zmq.LINGER,0) #Conflate keeps only the last message, we only need last servo position requested.
        sock_cmd_rcv.setsockopt(zmq.CONFLATE,True)
        sock_cmd_rcv.setsockopt(zmq.RCVTIMEO,1)
        sock_cmd_rcv.connect(rcv_sock_ip)

        #And this one is to exchange heartbeats.
        t_out = 0.5 #if no message brocker heartbeats in that time, emergency
        sock_hb_exchange = context.socket(zmq.REP)
        sock_hb_exchange.setsockopt(zmq.LINGER,0)
        sock_hb_exchange.setsockopt(zmq.SNDHWM,40)
        sock_hb_exchange.setsockopt(zmq.RCVHWM,40)
        sock_hb_exchange.setsockopt(zmq.RCVTIMEO,4000)
        sock_hb_exchange.setsockopt(zmq.SNDTIMEO,4000)
        sock_hb_exchange.connect(hb_sock_ip)

    except:
        print("------------------SERVO:ERROR: FAILED TO SET UP CONNECTION------------------")
        sock_cmd_rcv.close()
        sock_pos_send.close()
        sock_hb_exchange.close()
        context.term()
        print("SERVO: Networking closed")
        raise

    def close_networking():
        sock_cmd_rcv.close()
        sock_pos_send.close()
        sock_hb_exchange.close()
        context.term()

    def send_servo_pos(time_pos_read,servo_pos_deg):
        msg = pack_servo_pos_msg(servo1_id,time_pos_read,servo_pos_deg)
        sock_pos_send.send(msg)

    try:
        test_servo_port = '/dev/ttyS4'
        test_servo_id = 0x01
        TestServo = Servo(test_servo_port, test_servo_id)
        print("SERVO: Servo instance created")
    except:
        print("SERVO:ERROR: ------------------Servo creation failed. Closing------------------")
        close_networking()
        raise
    
    try:
        #First, wait for a heartbeat from the message brocker
        #before that, go to sleep
        #If no heartbeat, close the program, if there is a heartbeat, send one back, and start working
        t_hb_timeout = 1
        t_first_hb_pause = 0.1
        t_first_hb_timeout = 40 # Seconds, for manual start
        print("SERVO: Waiting for heartbeat")
        #I am trying to receive for t_first_hb_timeout seconds, if I don't get a heartbeat, I'll close
        init_mb_hb = None
        for i in range(int(t_first_hb_timeout/t_first_hb_pause)):
            try:
                time.sleep(t_first_hb_pause)
                init_mb_hb = sock_hb_exchange.recv(zmq.DONTWAIT)
                if init_mb_hb is not None:
                    break
            except zmq.error.Again:
                continue

        if init_mb_hb is not None:
            print(f"SERVO: Heartbeat received: {init_mb_hb}")
            sock_hb_exchange.send(struct.pack('2s',servo1_id))
        else:
            print("------------------SERVO:ERROR: No heartbeat received. Closing------------------")
            close_networking()
            raise

        print("------------------SERVO: Entering main loop------------------")
        time_since_last_hb = 0
        time_of_last_hb = time.time()
        time_of_last_send = 0
        hb_send_flag = True
        while True:
            print("SERVO: Looping")
            #Recieving desired servo position from servo_cmd_q and setting servo to it
            try:
                print("SERVO: recieving cmd")
                cmd_msg = sock_cmd_rcv.recv(zmq.DONTWAIT)
            except zmq.error.Again:
                cmd_msg = None

            #Exchanging heartbeats
            try:
                hb_msg_recv = sock_hb_exchange.recv(zmq.DONTWAIT)
            except zmq.error.Again:
                hb_msg_recv = None
                hb_send_flag = False
                time_since_last_hb = time.time() - time_of_last_hb

            if hb_msg_recv is not None:
                hb_send_flag = True
                time_of_last_hb = time.time()
                if hb_msg_recv != servo1_id:
                    print(f"SERVO:ERROR: Unexpected heartbeat. My id is {servo1_id}, recieved msg for {hb_msg_recv}")
                    raise Exception("Unexpected heartbeat")
            if hb_send_flag:    
                sock_hb_exchange.send(struct.pack('2s',servo1_id))

            #Setting servo position
            if (cmd_msg is not None):
                servo_id, time_msg_sent, servo_angle_req = unpack_servo_cmd_msg(cmd_msg)
                if servo1_id != servo_id:
                    print(f"SERVO:ERROR: Unexpected servo id. My id is {servo1_id}, recieved msg for {servo_id}")
                    raise Exception("Unexpected servo id")
                set_pos_err_code = TestServo.set_pos(servo_angle_req)
                if set_pos_err_code is not 0:
                    print("Didn't manage to set servo position")
            else:
                time.sleep(0.002)

            #Sending servo position to message brocker if havent done that in last 0.01 seconds
            time_since_last_send = time.time() - time_of_last_send; 
            if time_since_last_send > 0.01:
                servo_pos_deg, _ = TestServo.get_pos()
                time_pos_read = time.time()
                print("SERVO: Sending servo pos")
                send_servo_pos(time_pos_read,servo_pos_deg)
                time_of_last_send = time.time()

            if time_since_last_hb > t_hb_timeout:
                print(f"DATA MANAGER:ERROR: Servo 1 heartbeat timeout time since last hb: {time_since_last_hb}")
                raise Exception("Heartbeat timeout")
                    
    except KeyboardInterrupt:
        print("SERVO: Closed from keyboard")
        close_networking()
    except:
        print("------------------SERVO:ERROR:SOME ERROR IN MAIN LOOP------------------")
        close_networking()
        raise

if __name__ == '__main__' :
    main()
        



