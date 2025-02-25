import time
from opa_msg_library import pack_adc_state_msg, unpack_adc_state_msg 

def tfunc(x):
    return x+1

#I need to make sure the fields are of double type.
#And the same goes for other messages,
#otherwise, pack and unpack, might break, or maybe python handles it alredy...
adc_data_dict = {
    "militime": 1000.0,
    "absPressure": 2000.0,
    "absSenseTemp": 2000.0,
    "diffPressureDL": 3000.0,
    "diffSenseTempDL": 4000.0,
    "rearFlagAOA": 589.0,
    "frontFlagYaw": 15856
    }
time1 = time.time()
msg = pack_adc_state_msg(b'A1',100.0,adc_data_dict)
print(f"MSG: {msg}")
adc_id, msg_type, time_msg_sent, adc_data_dict_rx = unpack_adc_state_msg(msg)
time2 = time.time()
delt = time2 - time1
print("---------MSG rx---------")
print(f"ADC id:   {adc_id}")
print(f"msg type: {msg_type}")
print(f"time: {time_msg_sent}")
print(f"adc dict rx:  {adc_data_dict}")
print(f"delta time:  {delt}")



def test_tfunc():
    assert tfunc(3)  == 4
    assert tfunc(-1) == 0

test_tfunc()