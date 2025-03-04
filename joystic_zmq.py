#Joystic partion

import time
import zmq

from opa_msg_library import *
from joystic_module.SimpleJoystickInterface import SimpleJoystickInterface

verbose = True
def main():
    #Defining joystic config. id is used for messsages
    #ZMQ is goint to raise an exception if send or recieve is unsucesfull withing socket_timeout.
    #servo_max_freq is to not tax CPU to much. Just going to sleep for that much at the end. 
    jsk_id = b'JK'
    socket_timeout = 1000
    joystic_max_freq = 50

    if verbose: print("--------- Joystick: starting socket creation ---------")
    #Setting up sockets. PULL is type to recieve. PUSH to send.
    #LINGER 0 makes it close immidiatly when close is caleed for.
    #CONFLATE 1 keeps only the last message in the socket.
    #ip's are defined in data_agregator_zmq, all connections are to it.
    context = zmq.Context()

    #Each socket is supposed to recieve it's own type of message.
    #Socket for receiving joystic_cmd_msg
    jsk_ias_rx_sock = context.socket(zmq.PULL)
    jsk_ias_rx_sock.setsockopt(zmq.RCVTIMEO, socket_timeout) #Raize zmq.error.Again if didnt manage to send message during the timeout
    jsk_ias_rx_sock.setsockopt(zmq.LINGER, 0) # Discard pending messages on close
    jsk_ias_rx_sock.setsockopt(zmq.CONFLATE, 1) # Only keep the last message
    jsk_ias_rx_sock.connect('tcp://localhost:5550')

    #Socket to send joystic_state_msg
    jsk_pos_tx_sock = context.socket(zmq.PUSH)
    jsk_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
    jsk_pos_tx_sock.setsockopt(zmq.LINGER, 0)
    jsk_pos_tx_sock.setsockopt(zmq.CONFLATE, 1)
    jsk_pos_tx_sock.connect('tcp://localhost:5551')

    if verbose: print("-------Joystick: socket creation done -----------")

    def close_networking():
        jsk_ias_rx_sock.close()
        context.term()

    try:
        JoysticInteface = SimpleJoystickInterface()
        time.sleep(1)
        if verbose: print("Joystic inteface created")
    except:
        if verbose: print("Joystic creation failed. Closing")
        close_networking()
        raise
    
    while True:
        #Trying to set force, if no new force msg available, just skip
        ias_msg_rxed  = True
        ias_msg_valid = True
        try:
            ias_msg = jsk_ias_rx_sock.recv(zmq.DONTWAIT)
        except zmq.Again:
            ias_msg_rxed = False

        if ias_msg_rxed:
            jsk_id_rxed, msg_type, time_msg_sent, ias = unpack_joystic_cmd_msg(ias_msg)
            #print(f"IAS msg recieved: {unpack_joystic_cmd_msg(ias_msg)}")
            if jsk_id != jsk_id_rxed : ias_msg_valid = False

        if ias_msg_valid and ias_msg_rxed:
            JoysticInteface.adjustForce(ias)

        #Trying to read jostic status
        pitch, roll = JoysticInteface.get_pitch_roll()
        time_now = time.time()
        state_msg = pack_joystic_state_msg(jsk_id,time_now,pitch,roll)
        jsk_pos_tx_sock.send(state_msg)
        print(f"Joystic state message: {unpack_joystic_state_msg(state_msg)}")

        time.sleep(1/joystic_max_freq)

if __name__ == '__main__':
    main()