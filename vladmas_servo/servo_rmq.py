import pika
import time
import struct
from DummyServo import Servo



def main():
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()
    #Its probably safer to have a json that contains all the exchanges, name and so on.
    channel.exchange_declare("servo_pos_exchange", exchange_type='fanout')
    channel.exchange_declare("servo_cmd_exchange",exchange_type = 'direct')
    channel.queue_declare("servo_cmd_q")
    channel.queue_bind(exchange="servo_cmd_exchange",queue="servo_cmd_q")
    print("Servo: exchanges created")

    test_servo_port = '/dev/ttyS4'
    test_servo_id = 0x01
    TestServo = Servo(test_servo_port, test_servo_id)
    print("Servo instance created")

    def decode_servo_cmd(msg_body):
        time_msg_sent, servo_angle_req = struct.unpack('dd',msg_body)
        return time_msg_sent, servo_angle_req
    def send_servo_cmd(time_pos_read,servo_pos_deg):
        msg = struct.pack('dd',time_pos_read,servo_pos_deg)
        channel.basic_publish(exchange="servo_pos_exchange",routing_key='',body=msg)

    time_to_send = True
    time_since_last_send = 0
    time_of_last_send = time.time()
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
                send_servo_cmd(time_pos_read,servo_pos_deg)
                time_of_last_send = time.time()

if __name__ == '__main__' :
    main()
        



