import zmq
from opa_msg_library import *
import time

verbose = True

def set_default_ops_push(socket,timeout):
    """
    Sets default options for push sockets
    """
    socket.setsockopt(zmq.SNDTIMEO, timeout)
    socket.setsockopt(zmq.LINGER, 0)
    socket.setsockopt(zmq.CONFLATE, 1)

def set_default_ops_pull(socket,timeout):
    """
    Sets default options for pull sockets 
    """
    socket.setsockopt(zmq.RCVTIMEO, timeout)
    socket.setsockopt(zmq.LINGER, 0)
    socket.setsockopt(zmq.CONFLATE, 1) 

def main():
    socket_timeout = 5000 #ms
    no_msg_timeout = 10000 #ms
    context = zmq.Context()

    if verbose: print("Setting up sockets")
    fc_s1_cm_tx_sock   = context.socket(zmq.PUSH)
    set_default_ops_push(fc_s1_cm_tx_sock,socket_timeout)
    fc_s1_cm_tx_sock.connect('tcp://localhost:5670')

    fc_s1_pos_rx_sock = context.socket(zmq.PULL)
    set_default_ops_pull(fc_s1_pos_rx_sock,socket_timeout)
    fc_s1_pos_rx_sock.connect('tcp://localhost:5671')

    fc_jsk_pos_rx_sock = context.socket(zmq.PULL)
    set_default_ops_pull(fc_jsk_pos_rx_sock,socket_timeout)
    fc_jsk_pos_rx_sock.connect('tcp://localhost:5672')

    fc_jsk_ias_tx_sock = context.socket(zmq.PUSH)
    set_default_ops_push(fc_jsk_ias_tx_sock,socket_timeout)
    fc_jsk_ias_tx_sock.connect('tcp://localhost:5673')
    if verbose: print("Sockets set up")

    #So, no what... For now, let's just poll, print recieved messages
    #And set servo position to joystic position again....
    if verbose: print("Creating poller and registering input socket")
    poller = zmq.Poller()
    input_sockets = [fc_s1_pos_rx_sock, fc_jsk_pos_rx_sock]
    for sock in input_sockets:
        poller.register(sock, zmq.POLLIN)
    
    if verbose: print("Entering the main loop of the flight computer")
    while True:
        poller_dict = dict(poller.poll(no_msg_timeout)) 
        if False: 
            print("poller_dict: ", poller_dict)
            print("Is it empty? ", poller_dict == {})
        if poller_dict == {} : raise Exception("Timeout, poll did not return in time, no messages within the timeout period. Abnormal behavior.")
        for i, sock in enumerate(input_sockets):
            s1_pos_rxed = False
            jsk_pos_rxed = False
            if sock in poller_dict and poller_dict[sock] == zmq.POLLIN:
                if False: print(f"Recieved message from socket {i}")
                msg = sock.recv()
                if sock is fc_s1_pos_rx_sock:
                    s1_pos_rxed = True
                else:
                    jsk_pos_rxed = True

                if s1_pos_rxed:
                    servo_pos_msg_uncpacked = unpack_servo_pos_msg(msg)
                    #print(f"Servo pos msg: {servo_pos_msg_uncpacked}")
                else:
                    jsk_pos_msg_unpacked = unpack_joystic_state_msg(msg)
                    print(f"Jsk pos msg in: {jsk_pos_msg_unpacked}")
                    servo_cmd_msg = pack_servo_cmd_msg(b'S1',time.time(),jsk_pos_msg_unpacked[3])
                    print(f"Servo cmd msg out:{unpack_servo_cmd_msg(servo_cmd_msg)}")
                    fc_s1_cm_tx_sock.send(servo_cmd_msg)
                    jsk_cmd_msg = pack_joystic_cmd_msg(b'JK',time.time(),20)
                    #print(f"Joystic cmd out: {unpack_joystic_cmd_msg(jsk_cmd_msg)}")
                    fc_jsk_ias_tx_sock.send(jsk_cmd_msg)

if __name__ == '__main__':
    main()