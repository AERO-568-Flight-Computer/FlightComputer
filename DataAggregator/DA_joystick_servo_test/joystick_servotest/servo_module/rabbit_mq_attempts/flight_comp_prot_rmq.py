import pika
import time
import struct
#Another way to make joystic and servo communicate.
#This time. this program is in between them
#It's a prototype of flight computer logic? partition


def main():
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        channel = connection.channel()
        #Its probably safer to have a json that contains all the exchanges, name and so on.

        #Exhanges and queues for the joystic
        #To read joystic position
        channel.exchange_declare("joystic_pos_exchange", exchange_type='fanout')
        channel.queue_declare("fc_joystic_pos_q")
        channel.queue_bind(exchange="joystic_pos_exchange",queue="fc_joystic_pos_q")

        #To send IAS
        channel.exchange_declare("joystic_force_exchange",exchange_type = 'direct')
        channel.queue_declare("joystic_force_cmd_q")
        channel.queue_bind(exchange="joystic_force_exchange",queue="joystic_force_cmd_q")
        print("Joystic: exchanges created")

        #For the servo
        #To recieve servo position I guess separate excganges? for all servos?
        #Or different message headers? titles?, idk....
        channel.exchange_declare("servo_pos_exchange", exchange_type='fanout')
        channel.exchange_declare("servo_cmd_exchange",exchange_type = 'direct')
        channel.queue_declare("servo_cmd_q")
        channel.queue_declare("fc_servo_pos_q")
        channel.queue_bind(exchange="servo_cmd_exchange",queue="servo_cmd_q")
        channel.queue_bind(exchange="servo_pos_exchange",queue="fc_servo_pos_q")

    except:
        print("Failed to connect to rmq")
        channel.close() #If the channel was open, close it, if not, well, we fail again...
        raise


    def decode_joystic_force(msg_body):
        time_msg_sent, joystic_force_req = struct.unpack('dd',msg_body)
        return time_msg_sent, joystic_force_req

    def decode_servo_cmd(msg_body):
        time_msg_sent, servo_angle_req = struct.unpack('dd',msg_body)
        return time_msg_sent, servo_angle_req

    def close_networking():
        #I would like to delete exchange to let everyone know that this partition? failed.
        #it doesn't work. It deletes, but everyone who recieves the messages just hangs
        channel.exchange_delete(exchange="joystic_pos_exchange")
        channel.exchange_delete(exchange="joystic_force_exchange")
        channel.close()
        
    time_since_last_send = 0
    time_of_last_send = time.time()
    period_of_pos_send = 0.01
    try:
        while True:
                #Recieving desired joystic force (currently IAS placeholder) from joystic_force_cmd_q and setting joystic to it
                method_frame, header_frame, body = channel.basic_get('joystic_force_cmd_q')
                if (body is not None):
                    time_msg_sent, joystic_force_req = decode_joystic_force(body)
                    JoysticInteface.adjustForce(joystic_force_req)
                    channel.basic_ack(delivery_tag=method_frame.delivery_tag)
                else:
                    #In case basic_get takes resources
                    print("NoMSG")
                    time.sleep(0.005)

                #Sending joystic position to joystic exchange if havent done that in last period_of_pos_send seconds
                time_since_last_send = time.time() - time_of_last_send; 
                if time_since_last_send > period_of_pos_send:
                    pitch_deg,roll_deg, _ = JoysticInteface.get_pitch_roll()
                    time_pos_read = time.time()
                    send_joystic_pos(time_pos_read,pitch_deg,roll_deg)
                    time_of_last_send = time.time()
    except KeyboardInterrupt:
        print("Closed from keyaboard") 
        close_networking()
    except:
        print("Some error in the main loop")
        close_networking()

if __name__ == '__main__' :
    main()
