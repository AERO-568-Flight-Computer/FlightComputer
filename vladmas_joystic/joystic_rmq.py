import pika
import time
import struct
from DummySimpleJoysticInterface import SimpleJoysticInterface

def main():
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        channel = connection.channel()
        #Its probably safer to have a json that contains all the exchanges, name and so on.
        channel.exchange_declare("joystic_pos_exchange", exchange_type='fanout')
        channel.exchange_declare("joystic_force_exchange",exchange_type = 'direct')
        channel.queue_declare("joystic_force_cmd_q")
        channel.queue_bind(exchange="joystic_force_exchange",queue="joystic_force_cmd_q")
        print("Joystic: exchanges created")
    except:
        print("Failed to connect to rmq")
        channel.close() #If the channel was open, close it, if not, well, we fail again...
        raise

    def decode_joystic_force(msg_body):
        time_msg_sent, joystic_force_req = struct.unpack('dd',msg_body)
        return time_msg_sent, joystic_force_req
        
    def send_joystic_pos(time_pos_read, jsk_pitch_deg, jsk_roll_deg):
        msg = struct.pack('ddd',time_pos_read,jsk_pitch_deg,jsk_roll_deg)
        channel.basic_publish(exchange="joystic_pos_exchange",routing_key='',body=msg)

    def close_networking():
        #I would like to delete exchange to let everyone know that this partition? failed.
        #it doesn't work. It deletes, but everyone who recieves the messages just hangs
        channel.exchange_delete(exchange="joystic_pos_exchange")
        channel.exchange_delete(exchange="joystic_force_exchange")
        channel.close()
        
    try:
        JoysticInteface = SimpleJoysticInterface()
        time.sleep(1)
        print("Joystic inteface created")
    except:
        print("Joystic creation failed. Closing")
        close_networking()
        raise


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