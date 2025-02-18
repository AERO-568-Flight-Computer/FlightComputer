from opa_msg_library import *

import zmq
import time
import struct
from servo_module.DummyServo import Servo

verbose = True
def main():
    servo_id = b'S1'
    socket_timeout = 5000 # in milliseconds
    servo_max_freq = 10

    if verbose: print("Setting up sockets")
    context = zmq.Context()

    s1_cmd_rx_sock = context.socket(zmq.PULL)
    s1_cmd_rx_sock.connect('tcp://localhost:5560')
    s1_cmd_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    s1_cmd_rx_sock.setsockopt(zmq.LINGER, 0)
    s1_cmd_rx_sock.setsockopt(zmq.CONFLATE, 1)

    s1_pos_tx_sock = context.socket(zmq.PUSH)
    s1_pos_tx_sock.connect('tcp://localhost:5561')
    s1_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    s1_pos_tx_sock.setsockopt(zmq.LINGER, 0)
    s1_pos_tx_sock.setsockopt(zmq.CONFLATE,1)

    elevator_servo_port = '/dev/ttyS4'
    elevator_servo_id = 0x01
    servo = Servo(elevator_servo_port, elevator_servo_id)
    time.sleep(1)

    while True:
        if verbose: print("Main loop")
        set_pos_flag = True
        valid_cmd_msg_recieved = False

        #Send position
        pos_deg, _ = servo.get_pos()
        msg_time = time.time()
        pos_msg = pack_servo_pos_msg(servo_id,msg_time,pos_deg)
        s1_pos_tx_sock.send(pos_msg)

        #Try to recieve position
        try:
            pos_cmd_msg = s1_cmd_rx_sock.recv(zmq.DONTWAIT)
        except zmq.Again:
            set_pos_flag = False

        if set_pos_flag:
            servo_id_rxd, msg_type,time_msg_sent, servo_angle_req = unpack_servo_cmd_msg(pos_cmd_msg)
            if (servo_id_rxd == servo_id) and (msg_type == 'SC'): valid_cmd_msg_recieved = True
            else: raise Exception("Network is wrong, recieved unexpected message")

        if valid_cmd_msg_recieved:
            servo.set_pos(servo_angle_req)

        time.sleep(1/servo_max_freq)

if __name__ == '__main__' :
    main()