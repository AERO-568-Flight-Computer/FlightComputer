#This is the library that will be used to pack and unpack messages for zmq based networking
#In a sense its a protocol, that we plan to use to communicate between the partitions.
#ZeroMQ uses byte strings, so we'll use struct to pack and unpack the messages.

import struct
#Servo
def pack_servo_cmd_msg(servo_id,time_msg_sent, servo_angle_req):
    #servo_id is unick for a given servo, it's used to verify that the message is from who we expect.
    #b'SC' is crucial, it allows to check on unpack that the message of the correct type. b'SC' is msg_type.
    #msg_type must be unick for every message type.
    #time_msg_sent is time in seconds (time.time()) when message was send.
    # Important: time.time() has different resolution on different systems. Might be not precise enough for for differentiation/integration.
    format_str = '2s2sdd'
    msg = struct.pack(format_str,servo_id,b'SC',time_msg_sent, servo_angle_req)
    return msg

def unpack_servo_cmd_msg(msg: bytes):
    format_str = '2s2sdd'
    #Checking that the length of the msg is what's expect from format
    if len(msg) != struct.calcsize(format_str) : raise Exception("Invalid message")
    servo_id, msg_type,time_msg_sent, servo_angle_req = struct.unpack(format_str,msg)
    #Checking if the msg type is what expected
    if msg_type != b'SC': raise Exception("Invalid message")
    return servo_id,msg_type,time_msg_sent, servo_angle_req

def pack_servo_pos_msg(servo_id, time_pos_read,servo_pos_deg):
    format_str = '2s2sdd'
    msg  = struct.pack(format_str,servo_id,b'SP',time_pos_read,servo_pos_deg)
    return msg

def unpack_servo_pos_msg(msg: bytes):
    format_str = '2s2sdd'
    if len(msg) != struct.calcsize(format_str) : raise Exception("Invalid message")
    servo_id, msg_type, time_pos_read,servo_pos_deg = struct.unpack('2s2sdd',msg)
    if msg_type != b'SP': raise Exception("Invalid message")   
    return servo_id, msg_type,time_pos_read, servo_pos_deg

#Joystic
def pack_joystic_cmd_msg(jsk_id,time_msg_sent, ias):
    msg = struct.pack('2s2sdd',jsk_id,b'JC',time_msg_sent,ias)
    return msg

def unpack_joystic_cmd_msg(msg: bytes):
    format_str = '2s2sdd'
    if len(msg) != struct.calcsize(format_str) : raise Exception("Invalid message")
    jsk_id, msg_type,time_msg_sent, ias = struct.unpack(format_str,msg)
    if msg_type != b'JC': raise Exception("Invalid message")   
    return jsk_id,msg_type,time_msg_sent, ias

def pack_joystic_state_msg(jsk_id, time_msg_sent, pitch, roll):
    msg = struct.pack('2s2sddd',jsk_id, b'JS',time_msg_sent, pitch, roll)
    return msg

def unpack_joystic_state_msg(msg: bytes):
    format_str = '2s2sddd'
    if len(msg) != struct.calcsize(format_str) :
        #print("Message", msg)
        raise Exception("Invalid message")
    jsk_id, msg_type, time_msg_sent, pitch, roll = struct.unpack('2s2sddd',msg)
    if msg_type != b'JS': raise Exception("Invalid message")   
    return jsk_id, msg_type, time_msg_sent, pitch, roll

#Air data unit
def pack_adc_state_msg(adc_id,time_msg_sent,adc):
    #add: adc data dictionary
    #ADC data is a bit long. so instead of passing a lot of variables,
    #I'll pass a dictionary. A question I have is what happens when you unpack that 
    #   and return a dictionary though
    #   does it take longer than or not? I probably don;t care at this point...
    #adc_data_dict fiedls are the same last yer left:
    #adc_data_dict = {
    #     "militime": militime,
    #     "absPressure": absPressure,
    #     "absSenseTemp": absSenseTemp,
    #     "diffPressureDL": diffPressureDL,
    #     "diffSenseTempDL": diffSenseTempDL,
    #     "rearFlagAOA": rearFlagAOA,
    #     "frontFlagYaw": frontFlagYaw
    #     }

    #Just for code to be more readable, no meaning in parts
    msg_p1 = struct.pack('2s2sd',adc_id,b"AD",time_msg_sent)
    msg_p2 = struct.pack('ddd',adc["militime"],adc["absPressure"],adc["absSenseTemp"])
    msg_p3 = struct.pack('dd',adc["diffPressureDL"],adc["diffSenseTempDL"])
    msg_p4 = struct.pack('dd',adc["rearFlagAOA"],adc["frontFlagYaw"])
    msg = msg_p1 + msg_p2 + msg_p3 + msg_p4
    return msg

def unpack_adc_state_msg(msg: bytes):
    format_str = '2s2sdddddddd'
    if len(msg) != struct.calcsize(format_str) : raise Exception("Invalid message")

    msg_tuple = struct.unpack(format_str,msg)
    #Unpacking into dictionary, to see fields better for whomever uses the function
    #the dictionary is defined in pack_ad_msg
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
#It has multiple internal message types, which arrive at different frequencies. So need to manage that. (multiple types of our messages, or wait till some messages arrive,
# or send last slow internal messages at the highest freqeuency as well)

def pack_vn_state_msg(vn_id,time_msg_sent,vn):
    #add: adc data dictionary
    #ADC data is a bit long. so instead of passing a lot of variables,
    #I'll pass a dictionary. A question I have is what happens when you unpack that 
    #   and return a dictionary though
    #   does it take longer than or not? I probably don;t care at this point...
    #adc_data_dict fiedls are the same last yer left:
    #adc_data_dict = {
    #     "militime": militime,
    #     "absPressure": absPressure,
    #     "absSenseTemp": absSenseTemp,
    #     "diffPressureDL": diffPressureDL,
    #     "diffSenseTempDL": diffSenseTempDL,
    #     "rearFlagAOA": rearFlagAOA,
    #     "frontFlagYaw": frontFlagYaw
    #     }

    #Just for code to be more readable, no meaning in parts
    msg_p1 = struct.pack('2s2sd',vn_id,b"VN",time_msg_sent)
    msg_p2 = struct.pack('d',vn["TimeGPS"])
    msg_p3 = struct.pack('ddd',vn["Yaw"],vn["Pitch"],vn["Roll"])
    msg_p4 = struct.pack('ddd',vn["VelNed1"],vn["VelNed2"],vn["VelNed3"])
    msg = msg_p1 + msg_p2 + msg_p3 + msg_p4
    return msg

def unpack_vn_state_msg(msg: bytes):
    format_str = '2s2sdddddddd'
    if len(msg) != struct.calcsize(format_str) : raise Exception("Invalid message")

    msg_tuple = struct.unpack(format_str,msg)
    #Unpacking into dictionary, to see fields better for whomever uses the function
    #the dictionary is defined in pack_ad_msg
    vn_id         = msg_tuple[0]
    msg_type       = msg_tuple[1]
    if msg_type != b'AD': raise Exception("Invalid message")   
    time_msg_sent  = msg_tuple[2]
    TimeGPS       = msg_tuple[3]
    Yaw    = msg_tuple[4]
    Pitch   = msg_tuple[5]
    Roll = msg_tuple[6]
    VelNed1 = msg_tuple[7]
    VelNed2    = msg_tuple[8]
    VelNed3   = msg_tuple[9]

    vn_data_dict = {
         "TimeGPS": TimeGPS,
         "Yaw": Yaw,
         "Pitch": Pitch,
         "Roll": Roll,
         "VelNed1": VelNed1,
         "VelNed2": VelNed2,
         "VelNed3": VelNed3
         }
    return vn_id, msg_type, time_msg_sent, vn_data_dict