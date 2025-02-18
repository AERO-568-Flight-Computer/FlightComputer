import zmq

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
                if verbose: print(f"Receiving message from input socket {i}")
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

if __name__ == "__main__":
    socket_timeout = 1000 # in milliseconds

    if verbose: print("Setting up sockets")
    context = zmq.Context()
    rx_socket1 = context.socket(zmq.PULL)
    rx_socket1.bind('tcp://localhost:5556')
    rx_socket1.setsockopt(zmq.RCVTIMEO, socket_timeout)
    rx_socket1.setsockopt(zmq.LINGER, 0)

    rx_socket2 = context.socket(zmq.PULL)
    rx_socket2.bind('tcp://localhost:5557')
    rx_socket2.setsockopt(zmq.RCVTIMEO, socket_timeout)
    rx_socket2.setsockopt(zmq.LINGER, 0)

    tx_socket1 = context.socket(zmq.PUSH)
    tx_socket1.bind('tcp://localhost:5567')
    tx_socket1.setsockopt(zmq.SNDTIMEO, socket_timeout)
    tx_socket1.setsockopt(zmq.LINGER, 0)

    tx_socket2 = context.socket(zmq.PUSH)
    tx_socket2.bind('tcp://localhost:5568')
    tx_socket2.setsockopt(zmq.SNDTIMEO, socket_timeout)
    tx_socket2.setsockopt(zmq.LINGER, 0)
    if verbose: print("Sockets set up")

    rx_list = [rx_socket1, rx_socket2]
    tx_list = [tx_socket1, tx_socket2]
    routing_table = [[0, 1], [1]] # rx_socket1 -> tx_socket1, tx_socket_2, rx_socket2 -> tx_socket2
    try:
        switchboard(rx_list, tx_list, routing_table)
        print("-------------DATA MANAGER: EXITING DUE TO TIMEOUT-----------------")
        close_networking(rx_list, tx_list, context)
    except KeyboardInterrupt:
        print("-------------DATA MANAGER: EXITING DUE TO KEYBOARD INTERRUPT-----------------")
        close_networking(rx_list, tx_list, context)