import zmq
import time
import struct
from opa_msg_library import *

verbose = True
def switchboard(input_sockets, output_sockets, routing_table):
    """
    Routes messages from input sockets to output sockets based on the routing table.

    Args:
        input_sockets (list): List of ZMQ input sockets.
        output_sockets (list): List of ZMQ output sockets.
        routing_table (list): List of lists where each sublist corresponds to an input socket
                              and contains indices of output sockets to which messages should be sent.
    """
    no_msg_timeout = 45000 # in milliseconds
    # Create poller and register input sockets
    if verbose: print("Creating poller and registering input sockets")
    poller = zmq.Poller()
    for sock in input_sockets:
        poller.register(sock, zmq.POLLIN)
    
    if verbose: print("Entering main loop of the switchboard")
    while True:
        if verbose: print("Polling sockets")
        poller_dict = dict(poller.poll(no_msg_timeout)) 
        if verbose: 
            print("poller_dict: ", poller_dict)
            print("Is it empty? ", poller_dict == {})
        if poller_dict == {} : raise Exception("Timeout, poll did not return in time, no messages within the timeout period. Abnormal behavior.")
        for i, sock in enumerate(input_sockets):
            if sock in poller_dict and poller_dict[sock] == zmq.POLLIN:
                if verbose: print(f"Routing messages from socket {i}")
                message = sock.recv()
                for output_index in routing_table[i]:
                    print(f"To socket {output_index}")
                    output_sockets[output_index].send(message)

def close_networking(rx_list, tx_list,context):
    """
    Closes all sockets and the ZMQ context.
    """
    for sock in rx_list:
        sock.close()
    for sock in tx_list:
        sock.close()
    context.term()

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
    socket_timeout = 5000 # in milliseconds

    if verbose: print("Setting up sockets")
    context = zmq.Context()
    
    #Push pull is probably not the best socket type for this. pub sub might be better. This is because pub sub can send messages to more then one sub.
    #But how conflate works with that is unclear.
    #Joystic sockets
    jsk_ias_tx_sock = context.socket(zmq.PUSH)
    set_default_ops_push(jsk_ias_tx_sock,socket_timeout)    
    jsk_ias_tx_sock.bind('tcp://localhost:5550')

    jsk_pos_rx_sock = context.socket(zmq.PULL)
    set_default_ops_pull(jsk_pos_rx_sock,socket_timeout)
    jsk_pos_rx_sock.bind('tcp://localhost:5551')

    #Servo 1 sockets: elevator
    s1_cmd_tx_sock = context.socket(zmq.PUSH)
    set_default_ops_push(s1_cmd_tx_sock,socket_timeout)
    s1_cmd_tx_sock.bind('tcp://localhost:5560')

    s1_pos_rx_sock = context.socket(zmq.PULL)
    set_default_ops_pull(s1_pos_rx_sock,socket_timeout)
    s1_pos_rx_sock.bind('tcp://localhost:5561')

    #Flight computer sockets
    #For communications with servo 1
    fc_s1_cm_rx_sock = context.socket(zmq.PULL) #Flight computer send servo 1 desired position here
    set_default_ops_pull(fc_s1_cm_rx_sock,socket_timeout)    
    fc_s1_cm_rx_sock.bind('tcp://localhost:5670')
    
    fc_s1_pos_tx_sock = context.socket(zmq.PUSH) #Flight computer receives servo 1 current position from here
    set_default_ops_push(fc_s1_pos_tx_sock,socket_timeout)
    fc_s1_pos_tx_sock.bind('tcp://localhost:5671')

    #For communications with joystick
    fc_jsk_pos_tx_sock = context.socket(zmq.PUSH) #Flight computer receives joystick position from here
    set_default_ops_push(fc_jsk_pos_tx_sock,socket_timeout)    
    fc_jsk_pos_tx_sock.bind('tcp://localhost:5672')

    fc_jsk_ias_rx_sock = context.socket(zmq.PULL) #Flight computer sends IAS for the joystic here
    set_default_ops_pull(fc_jsk_ias_rx_sock,socket_timeout)    
    fc_jsk_ias_rx_sock.bind('tcp://localhost:5673')


    #For the logger
    #logger_tx_sock = context.socket(zmq.PUSH)
    #logger_tx_sock.bind('tcp://localhost:6100')
    #logger_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)

    input_sockets  = [jsk_pos_rx_sock, s1_pos_rx_sock, fc_s1_cm_rx_sock, fc_jsk_ias_rx_sock]
    output_sockets = [jsk_ias_tx_sock, s1_cmd_tx_sock, fc_s1_pos_tx_sock,fc_jsk_pos_tx_sock]
    routing_table =  [[3],           [2],          [1],            [0]]

    try:
        switchboard(input_sockets, output_sockets, routing_table)
    except Exception as e:
        print(e)
        raise
    finally:
        close_networking(input_sockets, output_sockets, context)

if __name__ == '__main__' :
    main()