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
    msg = struct.pack('2s2sdd',jsk_id,b'JC',time_msg_sent,ias)
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

#Air data unit
def pack_adc_state_msg(adc_id,time_msg_sent,adc):
    #add: adc data dictionary
    #ADC data is a bit long. so instead of passing a lot of variables,
    #I'll pass a dictionary. A question I have is what happens when you unpack that 
    #and return a dictionary though
    #does it take longer than or not? I probably don;t care at this point...
    #adc_data_dict fiedls are the same last yer people left:
    #adc_data_dict = {
    #     "militime": militime,
    #     "absPressure": absPressure,
    #     "absSenseTemp": absSenseTemp,
    #     "diffPressureDL": diffPressureDL,
    #     "diffSenseTempDL": diffSenseTempDL,
    #     "rearFlagAOA": rearFlagAOA,
    #     "frontFlagYaw": frontFlagYaw
    #     }
    msg_p1 = struct.pack('2s2sd',adc_id,b"AD",time_msg_sent)
    msg_p2 = struct.pack('ddd',adc["militime"],adc["absPressure"],adc["absSenseTemp"])
    msg_p3 = struct.pack('dd',adc["diffPressureDL"],adc["diffSenseTempDL"])
    msg_p4 = struct.pack('dd',adc["rearFlagAOA"],adc["frontFlagYaw"])
    msg = msg_p1 + msg_p2 + msg_p3 + msg_p4
    return msg

def unpack_adc_state_msg(msg):
    msg_tuple = struct.unpack('2s2sdddddddd',msg)
    adc_id         = msg_tuple[0]
    msg_type       = msg_tuple[1]
    if msg_type != b'AD': raise Exception("Invalid message")   
    time_msg_sent  = msg_tuple[2]
    militime       = msg_tuple[3]
    absPressure    = msg_tuple[4]
    absSenseTemp   = msg_tuple[5]
    diffPressureDL = msg_tuple[6]
    diffSenseTempDL= msg_tuple[7]
    rearFlagAOA    = msg_tuple[8]
    frontFlagYaw   = msg_tuple[9]

    adc_data_dict = {
         "militime": militime,
         "absPressure": absPressure,
         "absSenseTemp": absSenseTemp,
         "diffPressureDL": diffPressureDL,
         "diffSenseTempDL": diffSenseTempDL,
         "rearFlagAOA": rearFlagAOA,
         "frontFlagYaw": frontFlagYaw
         }
    return adc_id, msg_type, time_msg_sent, adc_data_dict

#Vector Nav
#Vector narv is the hardest to do, lots of fields.
#Probably has several types of messages it sends out.
#Probably, if it'so, It would be best to make a multipart zmq messages,
#First part being msg type. but for now for simplicity, I think just send what we need in one message when we get it.
#The info we need for our msg constains in several vector nav msg's. So we wait for all vector nav msgs's
#That contain the info, and then make our msg.