import serial
from typing import Dict

# Concept
# To get the group information, it would always be the byte right after the sync byte
# from that byte, we can get the group names and determine the number of groups that we need to look into to find the fields
# From the number of groups that will tell us the number of bytes after the group byte that we need to look into to find the fields and payload length

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
        6: 'DeltaTheta',
        7: 'DeltaVel',
        8: 'Mag',
        9: 'Accel',
        10: 'AngularRate',
        11: 'SensSat'
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
        10: 'GbssPosUncertainty',
        11: 'GnssVelUncertainty',
        12: 'GnssTimeUncertainty',
        13: 'GnssTimeInfo',
        14: 'GnssDop',
        15: 'GnssSatInfo',
        16: 'GnssRawMeas'
    },

    'Attitude': {
        1: 'Ypr',
        2: 'Quaternion',
        3: 'Dcm',
        4: 'MagNed',
        5: 'AccelNed',
        6: 'LinearBodyAcc',
        7: 'LinearAccelNed',
        8: 'YprU',
        9: 'Heave'
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
        14: 'GnssDop',
        15: 'GnssSatInfo',
        16: 'GnssRawMeas'
    }
}

'''Method is working'''
def findSyncByte(port, sync_byte):
    byte = port.read(1)
    while byte != sync_byte:
        byte = port.read(1)
    return byte

'''Method is working'''
def getMessage(port, sync_byte):
    message = []
    message.append(findSyncByte(port, sync_byte))
    byte = port.read(1)
    while byte != sync_byte:
        message.append(byte)
        byte = port.read(1)
    return message

'''Method is working'''
def getSize(message):
    return len(message)

'''Method is working'''
def findGroupByte(message):
    groupbyte = b''
    for byte in message:
        if byte == sync_byte:
            groupbyte = message[message.index(byte) + 1]
            break
    return groupbyte

'''Method is working'''
def getGroupInfo(groupbyte):
    binary_string = format(int.from_bytes(groupbyte, byteorder='big'), '08b')
    binary_string = binary_string[::-1]
    print(binary_string)
    
    active_groups = []
    
    for i, bit in enumerate(binary_string):
        if bit == '1':
            group_name = group_lookup_table.get(i+1)
            if group_name:
                active_groups.append(group_name)
    return active_groups

'''Method is working'''
def findGroupFieldBytes(message, active_groups):
    byte_count = 2
    start_index = 2
    group_field_bytes = []
    for _ in active_groups:
        fieldbytes = message[start_index:start_index+byte_count]
        group_field_bytes.append(fieldbytes[0] + fieldbytes[1])
        start_index += byte_count
    return group_field_bytes

# I think the issue that I'm getting is that its not reading the 16 bits correctly
# Example output:
# [b'\xfa', b'\x12', b'@', b'\x00', b'\x02', b'\x00', b'\x14', b'\x03', b'\x16', b'\x03', b'\t', b' ', b'm', b'\x00', b'\x0c', b'\xbd', b'\x9b', b'\xc2', b'\x88', b'\x95', b'\x97', b'>', b'\x9c', b'\xd4', b'`', b'\xbc', b'\xd1', b'\xdb']
# 01001000
# ['Time', 'Attitude']
# [b'@\x00', b'\x02\x00']
# 0000000000000010
# 0000000001000000
# {'Time': [], 'Attitude': []}

def getActiveFieldNames(active_groups, group_field_bytes, field_lookup_table):
    # This will hold the final active field names for each group
    active_fields_info = {}

    for group_name, field_bytes in zip(active_groups, group_field_bytes):
        # Convert the two field bytes to a binary string and reverse it
        # since the LSB corresponds to the first field
        field_status_bin = format(int.from_bytes(field_bytes, byteorder='big'), '016b')
        field_status_bin = field_status_bin[::-1]
        print(field_status_bin)
        
        # Extract the specific field names that are active for this group
        active_fields = []
        for i, bit in enumerate(field_status_bin):
            if bit == '1':
                field_name = field_lookup_table[group_name].get(i+1)
                if field_name:
                    active_fields.append(field_name)

        active_fields_info[group_name] = active_fields

    return active_fields_info

'''Method is working'''
def listToBytes(list_of_bytes):
    bytes = b''
    for byte in list_of_bytes:
        bytes += byte
    return bytes

'''Method is working'''
def removeByte(message, byte):
    new_message = list(message)
    i = 0
    while i < len(new_message):
        if new_message[i] == byte:
            new_message.pop(i)
            break
        i += 1
    return new_message


'''Method is working'''
def calculate_crc(data):
    crc = 0x0000
    for byte in data:
        crc = (crc >> 8) | (crc << 8) & 0xFFFF
        crc ^= byte
        crc ^= (crc & 0xFF) >> 4
        crc ^= (crc << 12) & 0xFFFF
        crc ^= ((crc & 0xFF) << 5) & 0xFFFF
    return crc

if __name__ == "__main__":

    port = serial.Serial('COM3', 115200)
    sync_byte = b'\xfa'

    try:
        while True:
            try:

                message = getMessage(port, sync_byte)

                # CRC Check to ensure message is not corrupted
                new_message = removeByte(message, sync_byte)
                data = listToBytes(new_message)
                crc = calculate_crc(data)

                if crc != 0:
                    raise ValueError("Message may be corrupted! CRC does not match.")
                
                print(message)
                
                groupbyte = findGroupByte(message)
                active_groups = getGroupInfo(groupbyte)
                print(active_groups)
                groupFields = findGroupFieldBytes(message, active_groups)
                print(groupFields)
                active_fields_info = getActiveFieldNames(active_groups, groupFields, field_lookup_table)
                print(active_fields_info)

            except (ValueError, IndexError) as error:
                print(error)
                continue

    except KeyboardInterrupt:
        print("Disconnected from VectorNav")
    finally:
        port.close()

