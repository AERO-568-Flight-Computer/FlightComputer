#Flight computer partition.
import zmq
from opa_msg_library import *
import time
from partitonManagerFunc import initialize
import sys

verbose = True

def set_default_ops_push(socket,timeout):
    """
    Sets default options for push sockets
    """
    socket.setsockopt(zmq.SNDTIMEO, timeout) #ZMQ raises ZMQ.Again if send failed withing timeout milliseconds
    socket.setsockopt(zmq.LINGER, 0)         #Closes the socket immidiatly when it's called for 
    socket.setsockopt(zmq.CONFLATE, 1)       #Keeps only the last message

def set_default_ops_pull(socket,timeout):
    """
    Sets default options for pull sockets 
    """
    socket.setsockopt(zmq.RCVTIMEO, timeout)
    socket.setsockopt(zmq.LINGER, 0)
    socket.setsockopt(zmq.CONFLATE, 1) 

def main():
    #Defining FC: Flight Computer.
    #ZMQ is goint to raise an exception if send or recieve is unsucesfull withing socket_timeout.
    #For now: FC waits for messages. Raises an exception if no messages incoming with no_msg_timeout

    socket_timeout = 5000 #ms
    no_msg_timeout = 10000 #ms
    context = zmq.Context()

    #Each socket is supposed to recieve it's own type of message.
    if verbose: print("Setting up sockets")
    fc_s1_cm_tx_sock   = context.socket(zmq.PUSH)
    set_default_ops_push(fc_s1_cm_tx_sock,socket_timeout)
    fc_s1_cm_tx_sock.connect('tcp://localhost:5670') #To send(tx) servo1 command message (servo_cmd_msg)

    fc_s1_pos_rx_sock = context.socket(zmq.PULL)
    set_default_ops_pull(fc_s1_pos_rx_sock,socket_timeout)
    fc_s1_pos_rx_sock.connect('tcp://localhost:5671') #To receive(rx) servo_pos_msg from servo 1

    fc_jsk_pos_rx_sock = context.socket(zmq.PULL) #To rx joystic_state_msg
    set_default_ops_pull(fc_jsk_pos_rx_sock,socket_timeout)
    fc_jsk_pos_rx_sock.connect('tcp://localhost:5672')

    fc_jsk_ias_tx_sock = context.socket(zmq.PUSH)
    set_default_ops_push(fc_jsk_ias_tx_sock,socket_timeout) #To tx joystic_cmd_msg
    fc_jsk_ias_tx_sock.connect('tcp://localhost:5673')

    fc_adc_cm_rx_sock = context.socket(zmq.PULL) #Flight computer send ADC command here
    set_default_ops_pull(fc_adc_cm_rx_sock,socket_timeout)    
    fc_adc_cm_rx_sock.connect('tcp://localhost:5681')

    fc_vn_cm_rx_sock = context.socket(zmq.PULL) #Flight computer send ADC command here
    set_default_ops_pull(fc_vn_cm_rx_sock,socket_timeout)    
    fc_vn_cm_rx_sock.connect('tcp://localhost:5691')

    if verbose: print("Sockets set up")

    #Poller allows to wait for messages from multiple sockets.
    #You register sockets with it, and then poller.poll() will block, until 
    #   at least one registered socket recieves a messages.
    #       at that point poller.poll() will return a dictionary that indicates which sockets recieved.
    if verbose: print("Creating poller and registering input socket")
    poller = zmq.Poller()
    input_sockets = [fc_s1_pos_rx_sock, fc_jsk_pos_rx_sock, fc_adc_cm_rx_sock, fc_vn_cm_rx_sock]
    for sock in input_sockets:
        poller.register(sock, zmq.POLLIN)
    
    initialize.initialize()

    #Waiting for at least one message to arrive, processing them when they do.
    if verbose: print("Entering the main loop of the flight computer")
    jsk_pos_value = 0
    adc_AOA_val = 0
    while True:
        poller_dict = dict(poller.poll(no_msg_timeout)) 
        if False: 
            print("poller_dict: ", poller_dict)
            print("Is it empty? ", poller_dict == {})
        if poller_dict == {} : raise Exception("Timeout, poll did not return in time, no messages within the timeout period. Abnormal behavior.")
        for i, sock in enumerate(input_sockets):
            if sock in poller_dict and poller_dict[sock] == zmq.POLLIN:
                msg = sock.recv()
                if False: print(f"Recieved message from socket {i}")

                if sock is fc_s1_pos_rx_sock:
                    #Recieved servo_pos_message
                    servo_pos_msg_uncpacked = unpack_servo_pos_msg(msg)
                    print(f"Servo pos msg: {servo_pos_msg_uncpacked}")
                elif sock is fc_jsk_pos_rx_sock:
                    #Recieved joystic postion message, 
                    # setting servo position to joystic position
                    # sending an IAS placeholder to joystic, for force feedback
                    jsk_pos_msg_unpacked = unpack_joystic_state_msg(msg)
                    
                    time1 = time.time()
                    print(f"{time1} : Jsk pos msg in: {jsk_pos_msg_unpacked}")
                    jsk_pos_value = jsk_pos_msg_unpacked[3].get('pitch')
                    # servo_cmd_msg = pack_servo_cmd_msg(b'S1',time.time(),jsk_pos_msg_unpacked[3])
                                         
                    # time1 = time.time()
                    # print(f"{time1} : Servo cmd msg out:{unpack_servo_cmd_msg(servo_cmd_msg)}")
                    # fc_s1_cm_tx_sock.send(servo_cmd_msg)

                    jsk_cmd_msg = pack_joystic_cmd_msg(b'JK',time.time(),20)
                    time1 = time.time()
                    print(f"{time1} : Joystic cmd out: {unpack_joystic_cmd_msg(jsk_cmd_msg)}")
                    fc_jsk_ias_tx_sock.send(jsk_cmd_msg)
                elif sock is fc_adc_cm_rx_sock:
                    # Recieve ADC message
                    adc_msg_unpacked = unpack_adc_state_msg(msg)[3]
                    adc_AOA_val = adc_msg_unpacked.get('rearFlagAOA');
                    # print(adc_msg_unpacked.get('rearFlagAOA'))
                    # print("HERE'S THE VALUE!!!!!")
                    # print(adc_AOA_val)
                    # servo_cmd_msg = pack_servo_cmd_msg(b'S1',time.time(),adc_msg_unpacked[8])
                elif sock is fc_vn_cm_rx_sock:
                    # Recieve VN message
                    vn_msg_unpacked = unpack_vn_state_msg(msg)
                    print(vn_msg_unpacked)
                else:
                    raise Exception("Should have not happened, recieved from an unexpected socket?")
            time1 = time.time()
            servo_pos_value = jsk_pos_value # + adc_AOA_val (this can be uncommented if you want the AOA vane to also control the servo motor. This can also be changed with a different value for dual control of the servo.)
            servo_cmd_msg = pack_servo_cmd_msg(b'S1',time1,servo_pos_value)
            print(f"{time1} : Servo cmd msg out:{unpack_servo_cmd_msg(servo_cmd_msg)}")
            fc_s1_cm_tx_sock.send(servo_cmd_msg)

if __name__ == '__main__':
    main()