#Elevator servo partition

from opa_msg_library import *

import zmq
import time
import struct
from servo_module.Servo import Servo
from partitonManagerFunc import initialize

verbose = True
def main():
    #Defining servo config. id is used for messsages
    #ZMQ is goint to raise an exception if send or recieve is unsucesfull withing socket_timeout.
    #servo_max_freq is to not tax CPU to much. Just going to sleep for that much at the end. 
    servo_id = b'S1'
    socket_timeout = 5000 # in milliseconds
    servo_max_freq = 500

    #Each socket is supposed to recieve it's own type of message.
    if verbose: print("Setting up sockets")
    #Setting up sockets. PULL is type to recieve. PUSH to send.
    #LINGER 0 makes it close immidiatly when close is caleed for.
    #CONFLATE 1 keeps only the last message in the socket.
    #ip's are defined in data_agregator_zmq, all connections are to it.
    context = zmq.Context()
    s1_cmd_rx_sock = context.socket(zmq.PULL)
    s1_cmd_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    s1_cmd_rx_sock.setsockopt(zmq.LINGER, 0)
    s1_cmd_rx_sock.setsockopt(zmq.CONFLATE, 1)
    s1_cmd_rx_sock.connect('tcp://localhost:5560') 

    s1_pos_tx_sock = context.socket(zmq.PUSH)
    s1_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    s1_pos_tx_sock.setsockopt(zmq.LINGER, 0)
    s1_pos_tx_sock.setsockopt(zmq.CONFLATE,1)
    s1_pos_tx_sock.connect('tcp://localhost:5561')

    #elevator_servo_port is COM port this particular servo is connected to.
    #elevator_servo_id role is unclear, but servo's internal communication (COM port based) need it.
    elevator_servo_port = '/dev/ttyS4'
    elevator_servo_id = 0x01
    servo = Servo(elevator_servo_port, elevator_servo_id)
    time.sleep(1)

    initialize.initialize()

    while True:
        if False: print("Main loop")
        set_pos_flag = True
        valid_cmd_msg_recieved = False

        #Send position
        pos_deg, _ = servo.get_pos()
        msg_time = time.time()
        pos_msg = pack_servo_pos_msg(servo_id,msg_time,pos_deg)
        #print(f"Pos msg out: {unpack_servo_pos_msg(pos_msg)}")
        s1_pos_tx_sock.send(pos_msg)
        time1 = time.time()
        print(f"{time1} : Position message out: {unpack_servo_pos_msg(pos_msg)}")

        #Try to recieve position, if haven't arrived, just continue to send current position.
        try:
            pos_cmd_msg = s1_cmd_rx_sock.recv(zmq.DONTWAIT)
        except zmq.Again:
            set_pos_flag = False

        if set_pos_flag:
            servo_id_rxd, msg_type,time_msg_sent, servo_angle_req = unpack_servo_cmd_msg(pos_cmd_msg)
            time1 = time.time()
            print(f"{time1} : Command message in: {unpack_servo_cmd_msg(pos_cmd_msg)}")
            servo.set_pos(servo_angle_req)

        #Sleeping to not tax the CPU too much.
        time.sleep(1/servo_max_freq)

if __name__ == '__main__' :
    main()