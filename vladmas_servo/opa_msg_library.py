#This is the library that will be used to pack and unpack messages for zmq based networking
#In a sense its a protocol, that we'll use to communicate between the partitions.
#ZeroMQ uses byte strings, so we'll use struct to pack and unpack the messages.
#It allows for multi part messages, so the first part is goint to be the id

import struct

def pack_servo_cmd_msg(servo_id,time_msg_sent, servo_angle_req):
    msg = struct.pack('2Sdd',servo_id,time_msg_sent, servo_angle_req)
    return msg

def unpack_servo_cmd_msg(msg):
    servo_id, time_msg_sent, servo_angle_req = struct.unpack('2sdd',msg)
    return servo_id, time_msg_sent, servo_angle_req

def pack_servo_pos_msg(servo_id, time_pos_read,servo_pos_deg):
    msg  = struct.pack('2sdd',servo_id,time_pos_read,servo_pos_deg)
    return msg

def unpack_servo_pos_msg(msg):
    servo_id, time_pos_read,servo_pos_deg = struct.unpack('2sdd',msg)
    return servo_id, time_pos_read, servo_pos_deg