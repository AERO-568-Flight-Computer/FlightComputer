import pika,sys,os
import struct
import time

def main():
    #Recieving
    connection = pika.BlockingConnection(pika.ConnectionParameters(host = 'localhost'))
    channel = connection.channel()
    channel.exchange_declare("servo_pos_exchange", exchange_type='fanout')
    channel.queue_declare("idk")
    channel.queue_bind("idk","servo_pos_exchange")

    def callback(ch,method,properties,body):
        print(f"[x] Received")
        double1,double2 = struct.unpack('dd',body)
        print(double1, '  ', double2)
        time.sleep(0.01)
        channel.basic_ack(delivery_tag=method.delivery_tag)

    
    channel.basic_consume(queue='idk',
                        on_message_callback=callback)
    print("[*] Waiting for messages.")
    channel.start_consuming()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("Keyboard interrupt")
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)


