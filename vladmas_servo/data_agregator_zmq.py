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

def main():
    socket_timeout = 1000 # in milliseconds

    if verbose: print("Setting up sockets")
    context = zmq.Context()

    #Joystic sockets
    jsk_ias_tx_sock = context.socket(zmq.PUSH)
    jsk_ias_tx_sock.bind('tcp://localhost:5550')
    jsk_ias_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout) #Raize zmq.error.Again if didnt manage to send message during the timeout
    jsk_ias_tx_sock.setsockopt(zmq.LINGER, 0) # Discard pending messages on close
    jsk_ias_tx_sock.setsockopt(zmq.CONFLATE, 1) # Only keep the last message

    jsk_pos_rx_sock = context.socket(zmq.PULL)
    jsk_pos_rx_sock.bind('tcp://localhost:5551')
    jsk_ias_tx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    jsk_pos_rx_sock.setsockopt(zmq.LINGER, 0)
    jsk_pos_rx_sock.setsockopt(zmq.CONFLATE, 1)

    #Servo 1 sockets: elevator
    s1_cmd_tx_sock = context.socket(zmq.PUSH)
    s1_cmd_tx_sock.bind('tcp://localhost:5560')
    s1_cmd_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    s1_cmd_tx_sock.setsockopt(zmq.LINGER, 0)
    s1_cmd_tx_sock.setsockopt(zmq.CONFLATE, 1) #The servo needs to know only the last position command

    s1_pos_rx_sock = context.socket(zmq.PULL)
    s1_pos_rx_sock.bind('tcp://localhost:5561')
    s1_pos_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    s1_pos_rx_sock.setsockopt(zmq.LINGER, 0)
    s1_pos_rx_sock.setsockopt(zmq.CONFLATE, 1)

    #Flight computer sockets
    #For communications with servo 1
    fc_s1_cm_rx_sock = context.socket(zmq.PULL) #Flight computer send servo 1 desired position here
    fc_s1_cm_rx_sock.bind('tcp://localhost:5670')
    fc_s1_cm_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    fc_s1_cm_rx_sock.setsockopt(zmq.LINGER, 0)
    fc_s1_cm_rx_sock.setsockopt(zmq.CONFLATE,1) #Only keep the last message, i want to send only the last desired position to the servo
    
    fc_s1_pos_tx_sock = context.socket(zmq.PUSH) #Flight computer receives servo 1 current position from here
    fc_s1_pos_tx_sock.bind('tcp://localhost:5671')
    fc_s1_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    fc_s1_pos_tx_sock.setsockopt(zmq.LINGER, 0)
    fc_s1_pos_tx_sock.setsockopt(zmq.CONFLATE,1)

    #For communications with joystick
    fc_jsk_pos_tx_sock = context.socket(zmq.PUSH) #Flight computer receives joystick position from here
    fc_jsk_pos_tx_sock.bind('tcp://localhost:5672')
    fc_jsk_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    fc_jsk_pos_tx_sock.setsockopt(zmq.LINGER, 0)
    fc_jsk_pos_tx_sock.setsockopt(zmq.CONFLATE,1)

    fc_jsk_ias_rx_sock = context.socket(zmq.PULL) #Flight computer sends IAS for the joystic here
    fc_jsk_ias_rx_sock.bind('tcp://localhost:5573')
    fc_jsk_ias_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout)
    fc_jsk_ias_rx_sock.setsockopt(zmq.LINGER, 0)
    fc_jsk_ias_rx_sock.setsockopt(zmq.CONFLATE,1)

    #For the logger
    logger_tx_sock = context.socket(zmq.PUSH)
    logger_tx_sock.bind('tcp://localhost:6100')
    logger_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)

    input_sockets  = [jsk_pos_rx_sock, s1_pos_rx_sock, fc_s1_cm_rx_sock, fc_jsk_ias_rx_sock]
    output_sockets = [jsk_ias_tx_sock, s1_cmd_tx_sock, fc_s1_pos_tx_sock,fc_jsk_pos_tx_sock, logger_tx_sock]
    routing_table =  [[3,4],           [2,4],          [1,4],            [0,4]]

    try:
        switchboard(input_sockets, output_sockets, routing_table)
    except Exception as e:
        print(e)
        raise
    finally:
        close_networking(input_sockets, output_sockets, context)

if __name__ == '__main__' :
    main()