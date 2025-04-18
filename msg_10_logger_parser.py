import os
import struct
import numpy as np
import matplotlib.pyplot as plt
class Msg10LogParser:
    #See Stirling inceptor Engineering Interface Specification
    #For message fields name.
    #SDL PN 115 EIS 01 Issue 1
    def __init__(self,log_filename):
        self.log_filename = log_filename
        self.startbytes = b'startmsg'
        self.endbytes   = b'endmsg'

    def parseFile(self):
        #Loading the whole file
        logfile = open(self.log_filename,'br')
        binstr = logfile.read()

        #Separating into messages
        len_end = len(self.endbytes)
        messages = binstr.split(self.startbytes)
        vallid_msg_10s = []
        i = 0
        for message in messages:
            #If it actually has the right length... I neeed to check a lot of cases other than nominal....
            if message[-len_end:] == self.endbytes:
                valid_msg_10 = message[:-len_end]
                vallid_msg_10s.append(valid_msg_10)
                i  = i+1
        return vallid_msg_10s
    
    def msg_10s_2bytes(self,valid_msg_10s):
        msg_dict_bytes = {
            "Axis" : [],
            "MessageID" : [],
            "InceptorN" : [],
            "Status" : [],
            "Position" : [],
            "Force" : [],
            "MotorDemand": [],
            "DSS1": [],
            "DSS2": [],
            "AS1": [],
            "AS2": [],
            "AS3": [],
            "Version": [],
            "RawForceSO": [],
        }

        msg_dict = {
            "Axis" : [],
            "MessageID" : [],
            "InceptorN" : [],
            "Status" : [],
            "Position" : [],
            "Force" : [],
            "MotorDemand": [],
            "DSS1": [],
            "DSS2": [],
            "AS1": [],
            "AS2": [],
            "AS3": [],
            "Version": [],
            "RawForceSO": [],
        }

        i = 0
        for msg in valid_msg_10s:
            msg_dict_bytes["MessageID"]  .append(msg[0])
            msg_dict_bytes["Axis"]       .append(msg[1])
            msg_dict_bytes["InceptorN"]  .append(msg[2])
            msg_dict_bytes["Status"]     .append(msg[4:8])
            msg_dict_bytes["Position"]   .append(msg[8:12])
            msg_dict_bytes["Force"]      .append(msg[12:16])
            msg_dict_bytes["MotorDemand"].append(msg[16:20])
            msg_dict_bytes["DSS1"]       .append(msg[20:24])
            msg_dict_bytes["DSS2"]       .append(msg[24:28])
            msg_dict_bytes["AS1"]        .append(msg[28:32])
            msg_dict_bytes["AS2"]        .append(msg[32:36])
            msg_dict_bytes["AS3"]        .append(msg[36:40])
            msg_dict_bytes["Version"]    .append(msg[40:44])
            msg_dict_bytes["RawForceSO"] .append(msg[44:48])

            msg_dict["MessageID"]  .append(msg_dict_bytes["MessageID"][i]) #Raw bytes
            msg_dict["Axis"]       .append(msg_dict_bytes["Axis"][i])      #Raw bytes
            msg_dict["InceptorN"]  .append(msg_dict_bytes["InceptorN"][i]) #Raw bytes
            msg_dict["Status"]     .append(msg_dict_bytes["Status"][i])    #Raw bytes
            msg_dict["Position"]   .append(struct.unpack('f',msg_dict_bytes["Position"][i]))    #Float
            msg_dict["Force"]      .append(struct.unpack('f',msg_dict_bytes["Force"][i]))       #Float
            msg_dict["MotorDemand"].append(struct.unpack('f',msg_dict_bytes["MotorDemand"][i])) #Float
            msg_dict["DSS1"]       .append(msg_dict_bytes["DSS1"][i])#Raw bytes
            msg_dict["DSS2"]       .append(msg_dict_bytes["DSS2"][i])#Raw bytes
            msg_dict["AS1"]        .append(struct.unpack('f',msg_dict_bytes["AS1"][i])) #Float
            msg_dict["AS2"]        .append(struct.unpack('f',msg_dict_bytes["AS2"][i])) #Float
            msg_dict["AS3"]        .append(struct.unpack('f',msg_dict_bytes["AS3"][i])) #Float
            msg_dict["Version"]    .append(struct.unpack('f',msg_dict_bytes["Version"][i])) #Float
            msg_dict["RawForceSO"] .append(struct.unpack('f',msg_dict_bytes["RawForceSO"][i])) #Float
            i = i+1

        return msg_dict_bytes, msg_dict

    def split_by_axis(msg_dict):
        msg_dict_pitch = {
            "Axis" : [],
            "MessageID" : [],
            "InceptorN" : [],
            "Status" : [],
            "Position" : [],
            "Force" : [],
            "MotorDemand": [],
            "DSS1": [],
            "DSS2": [],
            "AS1": [],
            "AS2": [],
            "AS3": [],
            "Version": [],
            "RawForceSO": [],
        }
        msg_dict_roll = msg_dict_pitch
        alldictlen = len(msg_dict["MessageID"])

def main():
    Parser = Msg10LogParser("joystic_raw_log.binlog")
    files = os.listdir()
    valid_msg_10s = Parser.parseFile()
    msg_dict_bytes, msg_dict = Parser.msg_10s_2bytes(valid_msg_10s)
    plt.plot(np.array(msg_dict["Position"]))
    plt.plot(np.array(msg_dict["MotorDemand"]))
    plt.show()

if __name__ == '__main__':
    main()