import zmq
import time
import struct
from opa_msg_library import *

def main():
    try:
        #All the sockets are created here. message brocker is stable, so it binds, everybody else connects
        context = zmq.Context()

        #Servo 1
        servo1_id = b'S1'
        servo_1_pos_rcv_ip = 'tcp://localhost:5556'
        servo_1_cmd_send_ip = 'tcp://localhost:5557'
        servo1_hb_exchange_ip = 'tcp://localhost:5558'
        servo1_pos_rcv_socket = context.socket(zmq.PULL)
        servo1_pos_rcv_socket.setsockopt(zmq.LINGER,0)
        servo1_pos_rcv_socket.bind(servo_1_pos_rcv_ip)

        servo1_cmd_send_socket = context.socket(zmq.PUSH)
        servo1_cmd_send_socket.setsockopt(zmq.LINGER,0)
        servo1_cmd_send_socket.bind(servo_1_cmd_send_ip)

        servo1_hb_exchange_socket = context.socket(zmq.REQ)
        servo1_hb_exchange_socket.setsockopt(zmq.LINGER,0)
        servo1_hb_exchange_socket.setsockopt(zmq.SNDTIMEO,5000)
        servo1_hb_exchange_socket.setsockopt(zmq.RCVTIMEO,5000)
        servo1_hb_exchange_socket.bind(servo1_hb_exchange_ip)

        print("DATA MANAGER: Networking set up")
        time.sleep(0.2)
    except:
        print("------------------DATA MANAGER:ERROR: FAILED TO SET UP CONNECTION------------------")
        servo1_pos_rcv_socket.close()
        servo1_cmd_send_socket.close()
        servo1_hb_exchange_socket.close()
        context.term()
        print("DATA MANAGER: Networking closed")
        raise

    def close_networking():
        servo1_pos_rcv_socket.close()
        servo1_cmd_send_socket.close()
        servo1_hb_exchange_socket.close()
        context.term()
    
    #First, send a heartbeat to everybody
    #Send heartbeat.
    #Wait some relativly long time (1 second)
    #Receive heartbeats, if they are not there, print who is missing and close the program
    #Before we send the heartbeat, lets sleep for some realy long time, so everybody is ready.
    t_sleep_before_first_hb = 10 #20 seconds is for manual start
    t_first_hb_timeout = 1
    init_sucessful = True
    time.sleep(t_sleep_before_first_hb)
    try:
        print("------------------DATA MANAGER: Sending heartbeat------------------")
        servo1_hb_exchange_socket.send(struct.pack('2s',servo1_id))
        print(f"DATA MANAGER: Heartbeat {servo1_id} sent, waiting")
        #Copilot proposes to use the poller, but I idk how to use it.So I'll just sleep.
        time.sleep(t_first_hb_timeout)
        try:
            servo1_hb = None
            print("DATA MANAGER: Waiting for servo 1 init heartbeat")
            servo1_hb = servo1_hb_exchange_socket.recv(zmq.DONTWAIT)
            print(f"DATA MANAGER: Servo 1, ID: {servo1_id} heartbeat received: {servo1_hb}")
        except zmq.Again:
            init_sucessful = False
            print(f"DATA MANAGER:ERROR: SERVO 1, ID: {servo1_id} DID NOT RESPOND TO INITIAL HEARTBEAT")

        if not init_sucessful:
            close_networking()
            raise Exception("DATA MANAGER: ERROR: SOMEONE IS NOT UP")

    except KeyboardInterrupt:
        print("DATA MANAGER: Closed from keyboard")
        close_networking()

    except:
        print("------------------DATA MANAGER:ERROR: FAILED TO EXCHANGE HEARTBEATS------------------")
        close_networking()
        raise
    try:
        time_since_last_hb = 0
        hb_send_flag = True
        time_of_last_hb = time.time()
        print("------------------DATA MANAGER: Entering main loop------------------")
        while True:
            print("DATA MANAGER: Looping")
            #Exchanging heartbeats
            if hb_send_flag:
                try:
                    servo1_hb_exchange_socket.send(struct.pack('2s',servo1_id),zmq.DONTWAIT)
                except zmq.error.Again:
                    print("DATA MANAGER:ERROR: Heartbeat send failed")
                    raise
            try:
                hb_msg_recv = servo1_hb_exchange_socket.recv(zmq.DONTWAIT)
            except zmq.error.Again:
                hb_msg_recv = None
                hb_send_flag = False
                time_since_last_hb = time.time() - time_of_last_hb
            if hb_msg_recv is not None:
                time_of_last_hb = time.time()
                hb_send_flag = True
                if hb_msg_recv != servo1_id:
                    print(f"DATA MANAGER:ERROR: Unexpected heartbeat. Servo id is {servo1_id}, recieved msg for {hb_msg_recv}")
                    raise Exception("Unexpected heartbeat")
                
            if time_since_last_hb > 2:
                print("DATA MANAGER:ERROR: Servo 1 heartbeat timeout")
                raise Exception("Heartbeat timeout")

    except KeyboardInterrupt:
        print("DATA MANAGER: Closed from keyboard")
        close_networking()
    except:
        close_networking()
        raise
if __name__ == '__main__' :
    main()