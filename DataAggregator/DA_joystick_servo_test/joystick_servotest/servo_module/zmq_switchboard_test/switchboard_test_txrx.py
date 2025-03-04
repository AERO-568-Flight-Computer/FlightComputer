import zmq
import time

verbose = True

def close_networking():
    """
    Closes all sockets and the ZMQ context.
    """
    rx_socket1.close()
    rx_socket2.close()
    tx_socket1.close()
    tx_socket2.close()
    context.term()

if __name__ == "__main__":
    if verbose: print("Setting up sockets")
    context = zmq.Context()
    tx_socket1 = context.socket(zmq.PUSH)
    tx_socket1.connect('tcp://localhost:5556')
    tx_socket1.setsockopt(zmq.SNDTIMEO, 1000)
    tx_socket1.setsockopt(zmq.LINGER, 0)

    tx_socket2 = context.socket(zmq.PUSH)
    tx_socket2.connect('tcp://localhost:5557')
    tx_socket2.setsockopt(zmq.SNDTIMEO, 1000)
    tx_socket2.setsockopt(zmq.LINGER, 0)

    rx_socket1 = context.socket(zmq.PULL)
    rx_socket1.connect('tcp://localhost:5567')
    rx_socket1.setsockopt(zmq.RCVTIMEO, 1000)
    rx_socket1.setsockopt(zmq.LINGER, 0)

    rx_socket2 = context.socket(zmq.PULL)
    rx_socket2.connect('tcp://localhost:5568')
    rx_socket2.setsockopt(zmq.RCVTIMEO, 1000)
    rx_socket2.setsockopt(zmq.LINGER, 0)
    if verbose: print("Sockets set up")

    #Want to. send a message to rx_socket1. should be received by tx_socket1 and tx_socket2 according to routing table.
    #Send a message to rx_socket2. should be received by tx_socket2
    msg_to_tx_socket1 = b"msg1"
    msg_to_tx_socket2 = b"msg2"
    try:
        while True:
            print("Sending message on tx1")
            tx_socket1.send(msg_to_tx_socket1)
            try:
                print(f"Received message on rx1: {rx_socket1.recv()}")
                print(f"Received message on rx2: {rx_socket2.recv()}")
            except zmq.error.Again:
                print("Timeout on receive")
                pass

            print("Sending message on tx2")
            tx_socket2.send(msg_to_tx_socket2)
            try:
                print(f"Received message on rx1: {rx_socket1.recv()}")
                print(f"Received message on rx2: {rx_socket2.recv()}")
            except zmq.error.Again:
                print("Timeout on receive")
                pass

            time.sleep(1)
    except KeyboardInterrupt:
        close_networking()
