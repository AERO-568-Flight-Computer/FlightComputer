#This is the library that will be used to pack and unpack messages for zmq based networking
#In a sense its a protocol, that I plan to use to communicate between the partitions.
#ZeroMQ uses byte strings, so we'll use struct to pack and unpack the messages.

import struct
#Servo
def pack_servo_cmd_msg(servo_id,time_msg_sent, servo_angle_req):
    msg = struct.pack('2s2sdd',servo_id,b'SC',time_msg_sent, servo_angle_req)
    return msg

def unpack_servo_cmd_msg(msg):
    servo_id, msg_type,time_msg_sent, servo_angle_req = struct.unpack('2s2sdd',msg)
    if msg_type != b'SC': raise Exception("Invalid message")
    return servo_id,msg_type,time_msg_sent, servo_angle_req

def pack_servo_pos_msg(servo_id, time_pos_read,servo_pos_deg):
    msg  = struct.pack('2s2sdd',servo_id,b'SP',time_pos_read,servo_pos_deg)
    return msg

def unpack_servo_pos_msg(msg):
    servo_id, msg_type, time_pos_read,servo_pos_deg = struct.unpack('2s2sdd',msg)
    if msg_type != b'SP': raise Exception("Invalid message")   
    return servo_id, msg_type,time_pos_read, servo_pos_deg

#Joystic
def pack_joystic_cmd_msg(jsk_id,time_msg_sent, ias):
    msg = struct.pask('2s2sdd',jsk_id,b'JC',time_msg_sent,ias)
    return msg

def unpack_joystic_cmd_msg(msg):
    jsk_id, msg_type,time_msg_sent, ias = struct.unpack('2s2sdd',msg)
    if msg_type != b'JC': raise Exception("Invalid message")   
    return jsk_id,msg_type,time_msg_sent, ias

def pack_joystic_state_msg(jsk_id, time_msg_sent, pitch, roll):
    msg = struct.pack('2s2sddd',jsk_id, b'JS',time_msg_sent, pitch, roll)
    return msg

def unpack_joystic_state_msg(msg):
    jsk_id, msg_type, time_msg_sent, pitch, roll = struct.unpack('2s2sddd',msg)
    if msg_type != b'JS': raise Exception("Invalid message")   
    return jsk_id, msg_type, time_msg_sent, pitch, roll