import pika
import time
import struct
from DummyServo import Servo



def main():
    try:
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        channel = connection.channel()
        #Its probably safer to have a json that contains all the exchanges, name and so on.
        channel.exchange_declare("servo_pos_exchange", exchange_type='fanout')
        channel.exchange_declare("servo_cmd_exchange",exchange_type = 'direct')
        channel.queue_declare("servo_cmd_q")
        channel.queue_bind(exchange="servo_cmd_exchange",queue="servo_cmd_q")
        print("Servo: exchanges created")
    except:
        print("Failed to connect to rmq")
        channel.close() #If the channel was open, close it, if not, well, we fail again...
        raise


    def decode_servo_cmd(msg_body):
        time_msg_sent, servo_angle_req = struct.unpack('dd',msg_body)
        return time_msg_sent, servo_angle_req
        
    def send_servo_pos(time_pos_read,servo_pos_deg):
        msg = struct.pack('dd',time_pos_read,servo_pos_deg)
        channel.basic_publish(exchange="servo_pos_exchange",routing_key='',body=msg)

    def close_networking():
        #I would like to delete exchange to let everyone know that this partition? failed.
        #it doesn't work. It deletes, but everyone who recieves the messages just hangs
        channel.exchange_delete(exchange="servo_cmd_exchange")
        channel.exchange_delete(exchange="servo_pos_exchange")
        channel.close()
        
    
    try:
        test_servo_port = '/dev/ttyS4'
        test_servo_id = 0x01
        TestServo = Servo(test_servo_port, test_servo_id)
        print("Servo instance created")
    except:
        print("Servo creation failed. Closing")
        close_networking()
        raise


    time_since_last_send = 0
    time_of_last_send = time.time()
    try:
        while True:
                #Recieving desired servo position from servo_cmd_q and setting servo to it
                method_frame, header_frame, body = channel.basic_get('servo_cmd_q')
                if (body is not None):
                    _, servo_angle_req = decode_servo_cmd(body)
                    set_pos_err_code = TestServo.set_pos(servo_angle_req)
                    channel.basic_ack(delivery_tag=method_frame.delivery_tag)
                    if set_pos_err_code is not 0:
                        print("Didn't manage to set servo position")
                else:
                    #In case basic_get takes resources
                    print("NoMSG")
                    time.sleep(0.005)

                #Sending servo position to servo_pos_exchange if havent done that in last 0.01 seconds
                time_since_last_send = time.time() - time_of_last_send; 
                if time_since_last_send > 0.01:
                    servo_pos_deg, _ = TestServo.get_pos()
                    time_pos_read = time.time()
                    send_servo_pos(time_pos_read,servo_pos_deg)
                    time_of_last_send = time.time()
    except KeyboardInterrupt:
        print("Closed from keyaboard") 
        close_networking()
    except:
        print("Some error in the main loop")
        close_networking()

if __name__ == '__main__' :
    main()
        



