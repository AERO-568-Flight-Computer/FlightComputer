import zmq
import time
import struct
import sys

context = zmq.Context()

tx_socket1 = context.socket(zmq.PUSH)

tx_socket1.bind('tcp://localhost:5556')

tx_socket2 = context.socket(zmq.PUSH)
tx_socket2.bind('tcp://localhost:5557')

print(f"Socket types is: {type(tx_socket1)}")
tx_socket1_shadow = tx_socket1
print(f"Shadow socket types is: {type(tx_socket1_shadow)}")
print(f"id of tx_socket1: {id(tx_socket1)}, id of tx_socket1_shadow: {id(tx_socket1_shadow)}, are they the same? {id(tx_socket1) == id(tx_socket1_shadow)}")

#If I say set timeout on the shadow, does it set it on the original?
tx_socket1_shadow.setsockopt(zmq.SNDTIMEO, 10000)
print(f"After setting timeout for tx_socket1_shadow socket option id of tx_socket1: {id(tx_socket1)}, id of tx_socket1_shadow: {id(tx_socket1_shadow)}, are they the same? {id(tx_socket1) == id(tx_socket1_shadow)}")


counter = 0
while True:
    counter += 1
    if counter % 2 == 0:
        t1 = time.time()
        msg1 = struct.pack('4sd',b'msg1',t1) #4sf does not pack time.time() Although time.time() is a float, it is not a float32. It is a float64?
        #"For the 'f', 'd' and 'e' conversion codes, 
        # the packed representation uses the IEEE 754 binary32, binary64 or binary16 format (for 'f', 'd' or 'e' respectively), 
        # regardless of the floating-point format used by the platform."
        #So if a "float" the system uses is a float64, if I use f, it will just pack 32 bits of it?

        print(f"Sending msg1 at time {t1}. time is of type {type(t1)}. it's size is {sys.getsizeof(t1)}: {struct.unpack('4sd',msg1)}")
        tx_socket1.send(msg1)
        tx_socket1_shadow.send(msg1)
        print(f"After I send: id of tx_socket1: {id(tx_socket1)}, id of tx_socket1_shadow: {id(tx_socket1_shadow)}, are they the same? {id(tx_socket1) == id(tx_socket1_shadow)}")

    
    if counter % 10 == 0:
        t2 = time.time()
        msg2 = struct.pack('4sd',b'msg2',time.time())
        print(f"Sending msg2 at time {t2}. ime is of type {type(t2)}. it's size is {sys.getsizeof(t2)}: {struct.unpack('4sd',msg2)}")
        tx_socket2.send(msg2)
    time.sleep(1)
#A python object. Has more bytes than expected.
#For a double, I expect 8 bytes. Fo
#r a float. I expect 4 bytes.
#But it has 24 bytes. Why? I think its because there is more information in the object than just the number. https://stackoverflow.com/questions/63483245/how-does-python-manage-size-of-integers-and-floats