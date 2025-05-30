import serial
import struct
import time
from typing import Dict
from opa_msg_library import *
import zmq
from partitonManagerFunc import initialize
import sys
from colorama import Fore, Back, Style

print(Fore.GREEN)

initialize.initialize()

verbose = True
#Defining servo config. id is used for messsages
#ZMQ is goint to raise an exception if send or recieve is unsucesfull withing socket_timeout.
#servo_max_freq is to not tax CPU to much. Just going to sleep for that much at the end. 
vn_id = b'V1'
socket_timeout = 5000 # in milliseconds

#Each socket is supposed to recieve it's own type of message.
if verbose: print("Setting up sockets")
#Setting up sockets. PULL is type to recieve. PUSH to send.
#LINGER 0 makes it close immidiatly when close is caleed for.
#CONFLATE 1 keeps only the last message in the socket.
#ip's are defined in data_agregator_zmq, all connections are to it.

context = zmq.Context()
vn_pos_tx_sock = context.socket(zmq.PUSH)
vn_pos_tx_sock.setsockopt(zmq.SNDTIMEO, socket_timeout)
vn_pos_tx_sock.setsockopt(zmq.LINGER, 0)
vn_pos_tx_sock.setsockopt(zmq.CONFLATE,1)
vn_pos_tx_sock.connect('udp://localhost:5591')

'''-----------------------------Lookup Tables (Data Structure)-------------------------------'''

group_lookup_table: Dict[int, str] = {
    1: 'Common',
    2: 'Time',
    3: 'Imu',
    4: 'Gnss',
    5: 'Attitude',
    6: 'Ins',
    7: 'Gnss2'
}

field_lookup_table: Dict[str, Dict[int, str]] = {
    'Common': {
        1: 'TimeStartup',
        2: 'TimeGps',
        3: 'TimeSyncIn',
        4: 'Ypr',
        5: 'Quaternion',
        6: 'AngularRate',
        7: 'PosLla',
        8: 'VelNed',
        9: 'Accel',
        10: 'Imu',
        11: 'MagPres',
        12: 'Deltas',
        13: 'InsStatus',
        14: 'SyncInCnt',
        15: 'TimeGpsPps',
    },

    'Time': {
        1: 'TimeStartup',
        2: 'TimeGps',
        3: 'GpsTow',
        4: 'GpsWeek',
        5: 'TimeSyncIn',
        6: 'TimeGpsPps',
        7: 'TimeUtc',
        8: 'SyncInCnt',
        9: 'SyncOutCnt',
        10: 'TimeStatus'
    },

    'Imu': {
        1: 'ImuStatus',
        2: 'UncompMag',
        3: 'UncompAccel',
        4: 'UncompGyro',
        5: 'Temperature',
        6: 'Pres', # This one is weird. It's not in the Control Center and there is some contradictions in the manual for this parameter. Appears in the manual as a field,
        # but isn't a group field that you can select in both the manual and Control Center. You can also see its presence in the group field bytes in the message. There will be
        # a zero in the middle of the message where the pressure field should be.
        7: 'DeltaTheta',
        8: 'DeltaVel',
        9: 'Mag',
        10: 'Accel',
        11: 'AngularRate',
        12: 'SensSat' # I couldn't find this group fields payload size, Don't select
    },

    'Gnss': {
        1: 'TimeUtc',
        2: 'GpsTow',
        3: 'GpsWeek',
        4: 'NumSats',
        5: 'GnssFix',
        6: 'GnssPosLla',
        7: 'GnssPosEcef',
        8: 'GnssVelNed',
        9: 'GnssVelEcef',
        10: 'GnssPosUncertainty',
        11: 'GnssVelUncertainty',
        12: 'GnssTimeUncertainty',
        13: 'GnssTimeInfo',
        14: 'GnssDop', # If this group field is selected in Control Center, the message will be corrupted and the CRC will not match 0.
        # This could be because the Vector Nav currently does not have a GPS connected to it and it isn't getting any satellite info on the DOP, but I'm not sure.
        15: 'GnssSatInfo', # Don't Select this field. Not Programmed. Just Please Don't. PLEASE.
        16: 'GnssRawMeas' # Same with this one. Just don't.
        # The problem with this is that the size of the message changes signicantly when these fields are selected.
        # GNSSSatInfo and GnssRawMeas change in size depending on a variable N that is specified in the manual.
    },

    'Attitude': {
        1: 'Reserved', # Reserved byte, probably for the presence of GnssRawMeas and GnssSatInfo fields.
        2: 'Ypr',
        3: 'Quaternion',
        4: 'Dcm',
        5: 'MagNed',
        6: 'AccelNed',
        7: 'LinearBodyAcc',
        8: 'LinearAccelNed',
        9: 'YprU',
        10: 'Heave' # This is also a weird group field. It appears in Control Center as a selectable group field, but it isn't described in the manual. It seems to be
        # reserved group field.
    },

    'Ins': {
        1: 'InsStatus',
        2: 'PosLla',
        3: 'PosEcef',
        4: 'VelBody',
        5: 'VelNed',
        6: 'VelEcef',
        7: 'MagEcef',
        8: 'AccelEcef',
        9: 'LinAccelEcef',
        10: 'PosU',
        11: 'VelU'
    },

    'Gnss2': {
        1: 'TimeUtc',
        2: 'GpsTow',
        3: 'GpsWeek',
        4: 'NumSats',
        5: 'GnssFix',
        6: 'GnssPosLla',
        7: 'GnssPosEcef',
        8: 'GnssVelNed',
        9: 'GnssVelEcef',
        10: 'GnssPosUncertainty',
        11: 'GnssVelUncertainty',
        12: 'GnssTimeUncertainty',
        13: 'GnssTimeInfo',
        14: 'GnssDop', # If this group field is selected in Control Center, the message will be corrupted and the CRC will not match 0.
        # This could be because the Vector Nav currently does not have a GPS connected to it and it isn't getting any satellite info on the DOP, but I'm not sure.
        15: 'GnssSatInfo', # PLEASE DON'T SELECT THIS FIELD
        16: 'GnssRawMeas' # Same with this one. Just don't.
        # The problem with this is that the size of the message changes signicantly when these fields are selected.
        # GNSSSatInfo and GnssRawMeas change in size depending on a variable N that is specified in the manual.
    }
}

payload_lookup_table: Dict[str, int] = {
    
    # These are all of the fields payload sizes that I know work and assumes that there will be only one case/data for each of these fields. If we were to add
    # a 2nd GNSS system then we would have to add a new case for the GNSS fields such as GPSTow2, GpsWeek2, etc. This should also be reflected in the field_lookup_table.

    # Time
        'TimeStartup': 8,
        'TimeGps': 8,
        'TimeSyncIn': 8,
        'TimeGpsPps': 8,
        'TimeUtc': 8,
        'SyncInCnt': 4,
        'SyncOutCnt': 4,
        'TimeStatus': 1,

    # IMU
        'ImuStatus': 2,
        'UncompMag': 12,
        'UncompAccel': 12,
        'UncompGyro': 12,
        'Temperature': 4,
        'DeltaTheta': 16,
        'DeltaVel': 12,
        'Mag': 12,
        'Accel': 12,
        'AngularRate': 12,

    # GNSS
        'GpsTow': 8,
        'GpsWeek': 2,
        'NumSats': 1,
        'GnssFix': 1,
        'GnssPosLla': 24,
        'GnssPosEcef': 24,
        'GnssVelNed': 12,
        'GnssVelEcef': 12,
        'GnssPosUncertainty': 12,
        'GnssVelUncertainty': 4,
        'GnssTimeUncertainty': 4,
        'GnssTimeInfo': 2,

    # Attitude
        'Ypr': 12,
        'Quaternion': 16,
        'Dcm': 36,
        'MagNed': 12,
        'AccelNed': 12,
        'LinearBodyAcc': 12,
        'LinearAccelNed': 12,
        'YprU': 12,

    # INS
        'InsStatus': 2,
        'PosLla': 24,
        'PosEcef': 24,
        'VelBody': 12,
        'VelNed': 12,
        'VelEcef': 12,
        'MagEcef': 12,
        'AccelEcef': 12,
        'LinAccelEcef': 12,
        'PosU': 4,
        'VelU': 4,

}

data_type_lookup_table: Dict[str, tuple[str, int]] = {

    # This table will be used to determine the data type that is being read from the message
    # b = signed byte
    # B = unsigned byte
    # H = unsigned short (2 bytes)
    # h = signed short (2 bytes)
    # f = float
    # Q = unsigned long long (8 bytes)
    # q = signed long long (8 bytes)
    # d = double (8 bytes)
    # I = unsigned int (4 bytes)
    # i = signed int (4 bytes)

        # Time
        'TimeStartup': ('Q', 8),
        'TimeGps': ('Q', 8),
        'TimeSyncIn': ('Q', 8),
        'TimeGpsPps': ('Q', 8),
        'TimeUtc': ('b 5B H', 8), # The year is given as a signed byte year offset from the year 2000 and the miliseconds byte is given as a u16 data type
        'SyncInCnt': ('I', 4),
        'SyncOutCnt': ('I', 4),
        'TimeStatus': ('B', 1), # has bit flags *Not Coded*

    # IMU
        'ImuStatus': ('H', 2),
        'UncompMag': ('3f', 12),
        'UncompAccel': ('3f', 12),
        'UncompGyro': ('3f', 12),
        'Temperature': ('f', 4),
        'DeltaTheta': ('4f', 16),
        'DeltaVel': ('3f', 12),
        'Mag': ('3f', 12),
        'Accel': ('3f', 12),
        'AngularRate': ('3f', 12),

    # GNSS
        'GpsTow': ('Q', 8),
        'GpsWeek': ('H', 2),
        'NumSats': ('B', 1),
        'GnssFix': ('B', 1), # Has bit flags *Not Coded*
        'GnssPosLla': ('3d', 24),
        'GnssPosEcef': ('3d', 24),
        'GnssVelNed': ('3f', 12),
        'GnssVelEcef': ('3f', 12),
        'GnssPosUncertainty': ('3f', 12),
        'GnssVelUncertainty': ('f', 4),
        'GnssTimeUncertainty': ('f', 4),
        'GnssTimeInfo': ('B b', 2), # Has bit flags *Not Coded*

    # Attitude
        'Ypr': ('3f', 12),
        'Quaternion': ('4f', 16),
        'Dcm': ('9f', 36),
        'MagNed': ('3f', 12),
        'AccelNed': ('3f', 12),
        'LinearBodyAcc': ('3f', 12),
        'LinearAccelNed': ('3f', 12),
        'YprU': ('3f', 12),

    # INS
        'InsStatus': ('H', 2),
        'PosLla': ('3d', 24),
        'PosEcef': ('3d', 24),
        'VelBody': ('3f', 12),
        'VelNed': ('3f', 12),
        'VelEcef': ('3f', 12),
        'MagEcef': ('3f', 12),
        'AccelEcef': ('3f', 12),
        'LinAccelEcef': ('3f', 12),
        'PosU': ('f', 4),
        'VelU': ('f', 4)
}

'''-----------------------------Main Methods-------------------------------'''

def getMessage(port, sync_byte):
    message = []
    message.append(findSyncByte(port, sync_byte))
    byte = port.read(1)
    while byte != sync_byte:
        message.append(byte)
        byte = port.read(1)
    return message

def calculate_crc(data):
    crc = 0x0000
    for byte in data:
        crc = (crc >> 8) | (crc << 8) & 0xFFFF
        crc ^= byte
        crc ^= (crc & 0xFF) >> 4
        crc ^= (crc << 12) & 0xFFFF
        crc ^= ((crc & 0xFF) << 5) & 0xFFFF
    return crc

def getGroupInfo(groupbyte):
    binary_string = format(int.from_bytes(groupbyte, byteorder='big'), '08b')
    binary_string = binary_string[::-1]
    
    active_groups = []
    
    for i, bit in enumerate(binary_string):
        if bit == '1':
            group_name = group_lookup_table.get(i+1)
            if group_name:
                active_groups.append(group_name)
    return active_groups

def findGroupFieldBytes(message, active_groups):
    byte_count = 2
    start_index = 2
    group_field_bytes = []
    for _ in active_groups:
        fieldbytes = message[start_index:start_index+byte_count]
        group_field_bytes.append(fieldbytes[0] + fieldbytes[1])
        start_index += byte_count
    return group_field_bytes

def getActiveFieldNames(active_groups, group_field_bytes, field_lookup_table):
    active_fields_info = {}

    for group_name, field_bytes in zip(active_groups, group_field_bytes):
        byte1 = field_bytes[0]
        byte2 = field_bytes[1]
        byte1_flipped = int(format(byte1, '08b')[::-1], 2)
        byte2_flipped = int(format(byte2, '08b')[::-1], 2)

        # Combine the two bytes into a 16-bit binary number
        binary16 = (byte1_flipped << 8) | byte2_flipped
        field_status_bin = format(binary16, '016b')
        
        # Extract the specific field names that are active for this group
        active_fields = []
        for i, bit in enumerate(field_status_bin):
            if bit == '1':
                field_name = field_lookup_table[group_name].get(i+1)
                if field_name:
                    active_fields.append(field_name)

        active_fields_info[group_name] = active_fields

    return active_fields_info

def parse_and_print_data(payload_message, payload_sizes, active_fields_info):
    start_index = 0
    for group_name, fields in active_fields_info.items():
        for field in fields:
            type_format, size = data_type_lookup_table[field]
            data_bytes = payload_message[start_index:start_index+size]
            if ' ' in type_format:
                formats = type_format.split()
                results = []
                offset = 0
                for fmt in formats:
                    part_size = struct.calcsize(fmt)
                    part_bytes = data_bytes[offset:offset+part_size]
                    results.extend(struct.unpack(fmt, part_bytes))
                    offset += part_size
                unpacked_data = tuple(results)
            else:
                unpacked_data = struct.unpack(type_format, data_bytes)
            print(f"{field} ({group_name}):", unpacked_data)
            start_index += size

def parse_and_return_data(payload_message, payload_sizes, active_fields_info):
    start_index = 0

    data_to_return = []

    for group_name, fields in active_fields_info.items():
        for field in fields:
            type_format, size = data_type_lookup_table[field]
            data_bytes = payload_message[start_index:start_index+size]
            if ' ' in type_format:
                formats = type_format.split()
                results = []
                offset = 0
                for fmt in formats:
                    part_size = struct.calcsize(fmt)
                    part_bytes = data_bytes[offset:offset+part_size]
                    results.extend(struct.unpack(fmt, part_bytes))
                    offset += part_size
                unpacked_data = tuple(results)
            else:
                unpacked_data = struct.unpack(type_format, data_bytes)
            data_to_return.append(unpacked_data)
            start_index += size

    return(data_to_return)


'''-----------------------------Utility Methods-------------------------------'''

def findSyncByte(port, sync_byte):
    byte = port.read(1)
    while byte != sync_byte:
        byte = port.read(1)
    return byte

def getSize(message):
    return len(message)

def findGroupByte(message):
    groupbyte = b''
    for byte in message:
        if byte == sync_byte:
            groupbyte = message[message.index(byte) + 1]
            break
    return groupbyte

def removeByte(message, byte):
    new_message = list(message)
    i = 0
    while i < len(new_message):
        if new_message[i] == byte:
            new_message.pop(i)
            break
        i += 1
    return new_message

def getPayloadSizes(active_fields_info, payload_lookup_table):
    
    payload_sizes = []

    for group_name, fields in active_fields_info.items():
        for field in fields:
            payload_size = payload_lookup_table.get(field)
            if payload_size:
                payload_sizes.append(payload_size)
    return payload_sizes

def calculatePayloadSize(payload_sizes):
    return sum(payload_sizes)

def removeHeader(message, active_groups):
    header_size = 2 + len(active_groups) * 2
    payload_message = message[header_size:]
    # Remove the 2 hex bytes that are at the end of the message (CRC bytes)
    return payload_message[:-2]

def joinBytes(bytes):
    joined_bytes = b''.join(bytes)
    return joined_bytes

'''-----------------------------Main-------------------------------'''

if __name__ == "__main__":

    port = serial.Serial('/dev/ttyUSB0', 115200) # Port may be changed depending on where you plug the VectorNav into
    sync_byte = b'\xfa'

    try:
        while True:
            try:
                message = getMessage(port, sync_byte) # Get the message
                # CRC Check to ensure message is not corrupted
                # *Note: From reading the manual, the CRC is caluclated from the byte after the sync byte to the end of the message.
                #        VectorNav makes it easy to check the CRC because when this calculation is done, the CRC should always be 0 for a valid message.
                new_message = removeByte(message, sync_byte) # removes the sync byte
                data = joinBytes(new_message) # joins the bytes into a single byte string
                crc = calculate_crc(data) # calculates the CRC

                # print(crc)
                if crc != 0:
                    raise ValueError("Message may be corrupted! CRC does not match.")
                
                # getting the group/groupfield info
                groupbyte = findGroupByte(message) # gets the group byte
                active_groups = getGroupInfo(groupbyte) # gets the active groups from the group byte
                groupFields = findGroupFieldBytes(message, active_groups) # gets the active group field bytes from the message
                active_fields_info = getActiveFieldNames(active_groups, groupFields, field_lookup_table) # gets the active fields from the active group field bytes
                
                # Extracting the payload from the active groups/groupfields
                payload_sizes = getPayloadSizes(active_fields_info, payload_lookup_table)
                payload_size = calculatePayloadSize(payload_sizes)
                payload_message = removeHeader(message, active_groups) # This gets rid of the header which contains the syncbyte, groupbyte, and groupfield bytes and the CRC bytes at the end

                # Prints data in a readable format on the terminal
                # print_data = parse_and_print_data(joinBytes(payload_message), payload_sizes, active_fields_info)
                # print(print_data)
                # time.sleep(1)
                # '''This needs work because i'm not sure how other devices will read the data yet.'''
                # # data that is sent to other devices to be used
                # VectorNav_data = joinBytes(payload_message)

                vn_data_list = parse_and_return_data(joinBytes(payload_message), payload_sizes, active_fields_info)

                TimeGPS = vn_data_list[0]
                ypr = vn_data_list[1]
                velned = vn_data_list[2]

                dataDictionary = {"TimeGps": TimeGPS, "ypr": ypr, "velned": velned}

                # print(type(dataDictionary["TimeGps"]))

                msg = pack_vn_state_msg(b'V1', time.time(), dataDictionary)

                # print(msg)

                # pack_vn_state_msg(vn_id,time_msg_sent,vn)
                vn_pos_tx_sock.send(msg)
                time1 = time.time()
                sys.stdout.write(f"\r\033[3A{'TimeGps | '+str(unpack_vn_state_msg(msg)[3].get('TimeGps')):<80}\n{'ypr | '+str(unpack_vn_state_msg(msg)[3].get('ypr')):<80}\n{'velned | '+str(unpack_vn_state_msg(msg)[3].get('velned')):<80}")  # <80 ensures at least 80 characters wide, padding with spaces
                sys.stdout.flush()
            except (ValueError, IndexError) as error:
                # print(error)
                continue

            time.sleep(0.1)

            #Reset the input buffer to prevent overflow
            port.reset_input_buffer()

    except KeyboardInterrupt:
        print("Disconnected from VectorNav")
    finally:
        port.close()