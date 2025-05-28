import struct
import warnings
import opa_msg_library as msl

DELIMBYTES   = b'delim_123' #delimiters for the log entries
DELIMTIME    = b'delt' #delimiter for time tag
SESSIONSTART = b'SESSIONSTART' #delimiter for session


file = open('LOG_from_zmqlogger.binlog','br')
filestr = file.read()
sessions = filestr.split(SESSIONSTART)
for i,session in enumerate(sessions):
    logentries = session.split(DELIMBYTES)
    sessions[i] = logentries
    for j, logentry in logentries:
        taged_logentry = logentry.split(DELIMTIME)
        session[i][j] = taged_logentry

print(session)



#First unpack with correct function, and put into a list:
#each element: dict: {id:, field1:, field2:, etc}

parsed_msgs = []
for binmsg in binmsgs:

    if len(binmsg) < 4:
        warnings.warn("Message length is too small")
        continue
    msg_type = binmsg[2:4]
    msg_dict = {}
    msg_dict["Time loged"] = msg_time_loged
    if msg_type == b'SC':
        msg_tuple = msl.unpack_servo_cmd_msg(binmsg)
        msg_dict["Servo ID"] = msg_tuple(0)
        msg_dict["Type"]     = msg_tuple(1)
        msg_dict["Time"]     = msg_tuple(2)
        msg_dict["Angle desired"] = msg_tuple(3)
    elif msg_type == b'SP':
        msg_tuple = msl.unpack_servo_pos_msg(binmsg)
        msg_dict["Servo ID"] = msg_tuple(0)
        msg_dict["Type"]     = msg_tuple(1)
        msg_dict["Time"]     = msg_tuple(2)
        msg_dict["Servo position"] = msg_tuple(3)
    elif msg_type == b'JC':
        msg_tuple = msl.unpack_joystic_cmd_msg(binmsg)
        msg_dict["Joystic ID"] = msg_tuple(0)
        msg_dict["Type"]       = msg_tuple(1)
        msg_dict["Time"]       = msg_tuple(2)
        msg_dict["IAS"]        = msg_tuple(3) #Why is that attempt at force feedback there?
    elif msg_type == b'JS':
        msg_tuple = msl.unpack_joystic_state_msg(binmsg)
        msg_dict["Joystic ID"] = msg_tuple(0)
        msg_dict["Type"]       = msg_tuple(1)
        msg_dict["Time"]       = msg_tuple(2)
        msg_dict["Pitch"]      = msg_tuple(3)
        msg_dict["Roll"]       = msg_tuple(4)
    elif msg_type == b'AD':
        msg_tuple = msl.unpack_adc_state_msg(binmsg)
        msg_dict["ADC ID"] = msg_tuple(0)
        msg_dict["Type"]   = msg_tuple(1)
        msg_dict["Time"]   = msg_tuple(2)
        adc_data_dic = msg_tuple(3)
        msg_dict["militime"]    = adc_data_dic["militime"]
        msg_dict["absPressure"]     = adc_data_dic["absPressure"]
        msg_dict["absSenseTemp"]    = adc_data_dic["absSenseTemp"]
        msg_dict["diffPressureDL"]  = adc_data_dic["diffPressureDL"]
        msg_dict["diffSenseTempDL"] = adc_data_dic["diffSenseTempDL"]
        msg_dict["rearFlagAOA"]     = adc_data_dic["rearFlagAOA"]
        msg_dict["frontFlagYaw"]    = adc_data_dic["frontFlagYaw"]
    elif msg_type == b'VN':
        msg_tuple = msl.unpack_vn_state_msg(binmsg)
    else:
        warnings.warn("Unrecognised msg type")
        continue
    parsed_msgs.append(msg_dict)
    
