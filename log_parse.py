import struct
import warnings
import ZMQ_run.opa_msg_library as msl
import pandas as pd
from tkinter.filedialog import askopenfilename
from tkinter.filedialog import askdirectory, askdirectory

initial_dir = "/home/cp-opa/Desktop/FlightComputer/logs"

FILENAME = askopenfilename(
    filetypes=[("Binary Log", "*.binlog")],
    title="Select File to Parse",
    initialdir=initial_dir
)

DIROUT   = askdirectory(
    title="Select Location to Save",
    initialdir=initial_dir
    )

DELIMBYTES   = b'delim_123'
DELIMTIME    = b'delt'
SESSIONSTART = b'SESSIONSTART'

file = open(FILENAME,'br')
filestr = file.read()

sessions = filestr.split(SESSIONSTART)
session1 = sessions[1]  # Assuming we only want the first session

entries = session1.split(DELIMBYTES)
binmsgs = []
time_logged_list = []

# for entry in entries:
    # print(entry)
entries.remove(b'')  # Remove any empty entries

for entry in entries:
    splitentry = entry.split(DELIMTIME)
    time_raw = splitentry[1]
    binmsg = splitentry[0]
    binmsgs.append(binmsg)
    time_logged = struct.unpack('d',time_raw)
    time_logged = time_logged[0]
    time_logged_list.append(time_logged)
    # print(entry[2:4], ' time raw:', time_raw, 'time:', time_logged)
file.close()

#First unpack with correct function, and put into a list:
#each element: dict: {id:, field1:, field2:, etc}

print("Parsing...")

parsed_msgs = []
for i,binmsg in enumerate(binmsgs):

    if len(binmsg) < 4:
        warnings.warn("Message length is too small")
        continue
    msg_type = binmsg[2:4]
    msg_dict = {}
    if msg_type == b'SC':
        msg_dict["Time loged"] = time_logged_list[i]
        msg_tuple = msl.unpack_servo_cmd_msg(binmsg)
        msg_dict["ID"] = msg_tuple[0]
        msg_dict["Type"]     = msg_tuple[1]
        msg_dict["Time"]     = msg_tuple[2]
        msg_dict["Angle desired"] = msg_tuple[3]["servo_angle_req"]
        parsed_msgs.append(msg_dict)
    elif msg_type == b'SP':
        msg_dict["Time loged"] = time_logged_list[i]
        msg_tuple = msl.unpack_servo_pos_msg(binmsg)
        msg_dict["ID"] = msg_tuple[0]
        msg_dict["Type"]     = msg_tuple[1]
        msg_dict["Time"]     = msg_tuple[2]
        msg_dict["Servo position"] = msg_tuple[3]["servo_pos_deg"]
        parsed_msgs.append(msg_dict)
    elif msg_type == b'JC':
        msg_dict["Time loged"] = time_logged_list[i]    
        msg_tuple = msl.unpack_joystic_cmd_msg(binmsg)
        msg_dict["ID"] = msg_tuple[0]
        msg_dict["Type"]       = msg_tuple[1]
        msg_dict["Time"]       = msg_tuple[2]
        msg_dict["IAS"]        = msg_tuple[3] #Why is attempt at force feedback here?
        parsed_msgs.append(msg_dict)
    elif msg_type == b'JS':
        msg_dict["Time loged"] = time_logged_list[i]    
        msg_tuple = msl.unpack_joystic_state_msg(binmsg)
        msg_dict["ID"] = msg_tuple[0]
        msg_dict["Type"]       = msg_tuple[1]
        msg_dict["Time"]       = msg_tuple[2]
        msg_dict["Pitch"]      = msg_tuple[3]["pitch"]
        msg_dict["Roll"]       = msg_tuple[3]["roll"]
        parsed_msgs.append(msg_dict)
    elif msg_type == b'AD':
        msg_dict["Time loged"] = time_logged_list[i]
        msg_tuple = msl.unpack_adc_state_msg(binmsg)
        msg_dict["ID"] = msg_tuple[0]
        msg_dict["Type"]   = msg_tuple[1]
        msg_dict["Time"]   = msg_tuple[2]
        adc_data_dic = msg_tuple[3]
        msg_dict["militime"]    = adc_data_dic["militime"]
        msg_dict["absPressure"]     = adc_data_dic["absPressure"]
        msg_dict["absSenseTemp"]    = adc_data_dic["absSenseTemp"]
        msg_dict["diffPressureDL"]  = adc_data_dic["diffPressureDL"]
        msg_dict["diffSenseTempDL"] = adc_data_dic["diffSenseTempDL"]
        msg_dict["rearFlagAOA"]     = adc_data_dic["rearFlagAOA"]
        msg_dict["frontFlagYaw"]    = adc_data_dic["frontFlagYaw"]
        parsed_msgs.append(msg_dict)
    elif msg_type == b'VN':
        msg_tuple = msl.unpack_vn_state_msg(binmsg)
    else:
        warnings.warn("Unrecognised msg type")
        continue
# for parsed_msg in parsed_msgs:
    # print(parsed_msg)


hirerachical_msgs_dict = {}
# print('\n\n')
for parsed_msg in parsed_msgs:
    #First, make sure that hirerachical_msgs_dict[parsed_msg["ID"].decode()][parsed_msg["Type"].decode()] exists.
    #Then, add to it.
    # print(parsed_msg)

    time_logged = parsed_msg["Time loged"]
    msg_id      = parsed_msg["ID"].decode()
    msg_type    = parsed_msg["Type"].decode()
    if msg_id not in hirerachical_msgs_dict.keys():
        hirerachical_msgs_dict[msg_id] = {}
    if msg_type not in hirerachical_msgs_dict[msg_id].keys():
        hirerachical_msgs_dict[msg_id][msg_type] = pd.DataFrame()

    msg_dataframe = pd.DataFrame(parsed_msg,[time_logged])
    old_dataframe = hirerachical_msgs_dict[msg_id][msg_type]

    new_dataframe = pd.concat([old_dataframe,msg_dataframe])
    hirerachical_msgs_dict[msg_id][msg_type] = new_dataframe

for msg_id in hirerachical_msgs_dict.keys():
    filename = DIROUT + '/' + msg_id + '_log.xlsx'
    with pd.ExcelWriter(filename) as exwriter:
        for msg_type in hirerachical_msgs_dict[msg_id].keys():
            dataframe = hirerachical_msgs_dict[msg_id][msg_type]
            dataframe.to_excel(exwriter, sheet_name = msg_type)

    
