import zmq
import time
import struct
from DummyServo import Servo

def main():
    try:
        context = zmq.Context()
        #I'd need a file per servo, the idea is, we hardcode the topology, since, we do not need
        #to handle situation when some unknown nodes connect, the dissapear, so on.
        #We have a network, that is not limited by throuput, and all the nodes are known.
        #So, star topology.
        
        #Change socket ports MANUALY. and keep track of them, we don't have many node.
        #So we'll manage

        send_sock_ip = "tcp://*:5556"
        rcv_sock_ip = "tcp://*:5557"
        hb_sock_ip = "tcp://*:5558"

        # This one is to send position to the message brocker
        sock_pos_send = context.socket(zmq.REQ) # This one is to send position to the message brocker
        sock_pos_send.setsockopt(zmq.LINGER,0) #LINGER makes it so the socket waits that much milisecond before closing
        sock_pos_send.connect(send_sock_ip)

        # This one is to receive pos commands from the message brocker
        sock_cmd_rcv  = context.socket(zmq.REP)
        sock_cmd_rcv.setsockopt(zmq.LINGER,0)
        sock_cmd_rcv.connect(rcv_sock_ip)

        #And this one is to exchange heartbeats.
        t_out = 0.5 #if no message brocker heartbeats in that time, emergency
        sock_hb_exchange = context.socket(zmq.REP)
        sock_hb_exchange.setsockopt(zmq.LINGER,0)
        sock_hb_exchange.connect(hb_sock_ip)

    except:
        print("Failed to set up connection")
        sock_cmd_rcv.close()
        sock_pos_send.close()
        sock_hb_exchange.close()
        context.term()
        print("Networking closed")
        raise

    def unpack_servo_cmd(msg_body):
        time_msg_sent, servo_angle_req = struct.unpack('dd',msg_body)
        return time_msg_sent, servo_angle_req
        
    def pack_servo_pos(time_pos_read,servo_pos_deg):
        msg = struct.pack('dd',time_pos_read,servo_pos_deg)

    def unpack_servo_pos(msg_body):
        time_pos_read,servo_pos_deg = struct.unpack('dd',msg_body)
        return time_pos_read, servo_pos_deg
    
    def close_networking():
        sock_cmd_rcv.close()
        sock_pos_send.close()

    try:
        test_servo_port = '/dev/ttyS4'
        test_servo_id = 0x01
        TestServo = Servo(test_servo_port, test_servo_id)
        print("Servo instance created")
    except:
        print("Servo creation failed. Closing")
        context.term()
        raise

    try:
        while True:
                print("Hi, main servo loop here")
                time.sleep(0.2)
    except KeyboardInterrupt:
        print("Closed from keyboard")
        context.term()
    except:
        print("Some error in the main loop")
        context.term()

if __name__ == '__main__' :
    main()
        



