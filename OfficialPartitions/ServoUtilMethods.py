# Methods taken from volz_actuator.py

# generate and append CRC
def generate_crc(command):
    # 0xDD, 0x01, 0x14, 0x40 – new position +30°
    #command = [0xDD, 0x1F, 0x14, 0x40]
    #command = [0xDD, 0x1F, 0x0D, 0x3A]  # command, actuator ID, arg1, arg2

    crc = 0xFFFF  # init value

    for x in range(4):
        crc = ((command[x] << 8) ^ crc)

        for y in range(8):
            if (crc & 0x8000):
                crc = (crc << 1) ^ 0x8005
            else:
                crc = crc << 1

    crc &= 0xFFFF  # keep the result within 16 bits
    crc = "{:X}".format(crc)
    padded_crc = crc.zfill(4)

    # split checksum, add to argument list
    command.append(int(padded_crc[0:2], 16))
    command.append(int(padded_crc[2:4], 16))

    return command


# Index 0: Command code
# Index 1: Actuator ID
# Index 2: Argument 1
# Index 3: Argument 2
# Index 4: CRC High-Byte
# Index 5: CRC Low Byte
def deg2hex(deg):
    decimal_command_num = 19.2 * deg + 2048
    bin_str = format(int(decimal_command_num), '#014b')

    # split binary string
    indices = [2, 7, 14]
    bin_arg_lst = [bin_str[i:j] for i, j in zip(indices, indices[1:] + [None])]

    # convert binary -> int
    arg1 = int(bin_arg_lst[0], 2)
    arg2 = int(bin_arg_lst[1], 2)

    hex_val = [arg1, arg2]  # decimal translation of individual hex values
    return hex_val


def hex2deg(vals):
    # vals: array of 2 decimal values for servo position
    bin_arg_list = list()
    for val in vals:
        val_bin = bin(val)                      # '0b10011'
        val_bin_str = val_bin[2:len(val_bin)]   # '10011'
        bin_arg_list.append(val_bin_str)
    bin_str = '0b' + bin_arg_list[0] + bin_arg_list[1].zfill(7)
    deg = (int(bin_str, 2) - 2048) / 19.2
    return deg


# define actuator set position command
def build_pos_command(degree, actuator_ID=0x01):
    # 0x1F = broadcast
    base_command = [0xDD, actuator_ID]

    hex_val = deg2hex(degree)
    arg1 = hex_val[0]
    arg2 = hex_val[1]

    # add arguments to command list
    base_command.append(arg1)
    base_command.append(arg2)

    # generate checksum
    cmd = generate_crc(base_command)

    return cmd


def get_valid_input():
    while True:
        try:
            input_str = input("Enter one (to set) or two (to loop) integer values between -90 and 90 (separated by a space): ")
            degrees = [int(deg) for deg in input_str.split()]
            if all(-90 <= deg <= 90 for deg in degrees) and len(degrees) in [1, 2]:
                return degrees
            else:
                print("Invalid input. Please try again.")
        except ValueError:
            print("Invalid input. Please try again.")


# get input voltage data
def get_pwr_status(ser, actuator_ID=0x01):
    #cmd = [0xB1, 0X01, 0X00, 0X00, 0x54, 0x05]
    cmd = [0xB1, actuator_ID, 0X00, 0X00]
    # generate checksum
    cmd = generate_crc(cmd)

    ser.write(bytearray(cmd))
    rx = ser.read(12)  # cmd echo is first 6 bytes, response is second set of 6 bytes
    #print(rx)
    
    pwr_servo = int(hex(rx[8]), 16) * 0.2   # 200 mV per val, volts
    pwr_clutch = int(hex(rx[9]), 16) * 0.2 
    print("servo input voltage: ", pwr_servo, "V")
    print("clutch input voltage: ", pwr_clutch, "V")
    return pwr_servo, pwr_clutch


def get_pos(ser, actuator_ID=0x01):
    cmd = [0x92, actuator_ID, 0x00, 0x00]
    cmd = generate_crc(cmd)
    ser.write(bytearray(cmd))
    rx = ser.read(12)   # cmd echo is first 6 bytes, response is second set of 6 bytes
    if len(rx) != 12:
        return
    else:
        pos_hex = rx[8:10]
        pos_deg = hex2deg(pos_hex)
        return pos_deg, pos_hex