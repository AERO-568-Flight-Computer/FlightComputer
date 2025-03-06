import serial
import time
class Servo:
    def __init__(self,port_string,actuator_id = 0x01):
        #port string is the port at which the servo is going to be.
        #'/dev/ttyS4' is the port of the servo we run
        self.ser = serial.Serial(port_string, 115200, timeout=1)
        self.actuator_id = actuator_id

    # Methods taken from volz_actuator.py, in particular from ServoUtilMethods
    # generate and append CRC
    '''
    def make_servo_socket(self):
        time.sleep(2)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_address = ('localhost', 12300)
        self.sock.bind(server_address)
    '''

    def set_pos(self,servo_desired_pos_deg):
        #Checking power
        servo_power_status, clutch_power_status = self.run_power_diag()
        if (servo_power_status == 0) or (clutch_power_status == 0):
            print("Trying to set servo pos, cant:")
            if servo_power_status == 0:
                print("Servo power out")
            if clutch_power_status == 0:
                print("Clutch power out")
            return -1
        
        #Checking servo position limits
        angleLimMin = -55
        angleLimMax = 55
        if angleLimMin < servo_desired_pos_deg < angleLimMax:
            command = self._build_pos_command(servo_desired_pos_deg)
            self.ser.write(bytearray(command))
            rx = self.ser.read(12)
            return 0
        else:
            print("Cant set desired position. Out of servo limits")
            return -1
    
    def get_pos(self):
        #Checking power
        servo_power_status, clutch_power_status = self.run_power_diag()
        if (servo_power_status == 0) or (clutch_power_status == 0):
            print("Trying to set servo pos, cant:")
            if servo_power_status == 0:
                print("Servo power out")
            if clutch_power_status == 0:
                print("Clutch power out")
            return -1, -1 #First is the position, the second is error code.
        pos_deg, pos_hex = self._get_pos()
        return pos_deg, 0         

    def run_power_diag(self):
        #Is the servo good to just leave be? or do I need to read from it constantly?
        #power_status is 1 if it's powered on, 0 if not
        servo_power_status = 0
        clutch_power_status = 0
        for i in range(100):
            try:
                pwr_servo, pwr_clutch = self._get_pwr_status()
                if pwr_servo > 20:
                    servo_power_status = 1

                if pwr_clutch > 20:
                    clutch_power_status = 1

                if servo_power_status and clutch_power_status:
                    break
            except:
                print("SERVO DRIVER: ", "Error: Could not power status... Servo may not be turned on.")
        return servo_power_status, clutch_power_status 

    @staticmethod
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
    @staticmethod
    def _deg2hex(deg):
        decimal_command_num = 19.2 * deg + 2048
        bin_str = format(int(round(decimal_command_num)), '#014b')

        # split binary string
        indices = [2, 7, 14]
        bin_arg_lst = [bin_str[i:j] for i, j in zip(indices, indices[1:] + [None])]

        # convert binary -> int
        arg1 = int(bin_arg_lst[0], 2)
        arg2 = int(bin_arg_lst[1], 2)

        hex_val = [arg1, arg2]  # decimal translation of individual hex values
        return hex_val

    @staticmethod
    def _hex2deg(vals):
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
    def _build_pos_command(self, degree):
        # 0x1F = broadcast
        base_command = [0xDD, self.actuator_id]

        hex_val = Servo._deg2hex(degree)
        arg1 = hex_val[0]
        arg2 = hex_val[1]

        # add arguments to command list
        base_command.append(arg1)
        base_command.append(arg2)

        # generate checksum
        cmd = Servo.generate_crc(base_command)

        return cmd

# get input voltage data
    def _get_pwr_status(self):
        #cmd = [0xB1, 0X01, 0X00, 0X00, 0x54, 0x05]
        #print(self.actuator_id)
        cmd = [0xB1, self.actuator_id, 0X00, 0X00]
        # generate checksum
        cmd = Servo.generate_crc(cmd)
        self.ser.write(bytearray(cmd))
        rx = self.ser.read(12)  # cmd echo is first 6 bytes, response is second set of 6 bytes
        #print("Get power status Command")
        #print(cmd)
        #print("Get power status Reply")
        #print(rx)
            
        pwr_servo = int(hex(rx[8]), 16) * 0.2   # 200 mV per val, volts
        pwr_clutch = int(hex(rx[9]), 16) * 0.2 
        #print("servo input voltage: ", pwr_servo, "V")
        #print("clutch input voltage: ", pwr_clutch, "V")
        return pwr_servo, pwr_clutch

    def _get_pos(self):
        cmd = [0x92, self.actuator_id, 0x00, 0x00]
        cmd = Servo.generate_crc(cmd)
        self.ser.write(bytearray(cmd))
        rx = self.ser.read(12)   # cmd echo is first 6 bytes, response is second set of 6 bytes
        if len(rx) != 12:
            return
        else:
            pos_hex = rx[8:10]
            pos_deg = Servo._hex2deg(pos_hex)
            return pos_deg, pos_hex

def main():
    #Runs test if directly called
    elevator_servo_port = '/dev/ttyS4'
    elevator_servo_id = 0x01
    ElevatorServo = Servo(elevator_servo_port, elevator_servo_id)
    time.sleep(2)
    positions = [-50, -25, 0, 25, 50]
    delay = 0.5

    while True:
        for pos in positions:
            #print("Tryuing to set postion")
            set_pos_err_code = ElevatorServo.set_pos(pos)
            time.sleep(delay)      
            if set_pos_err_code != 0:
                #print("set_pos failed with exit code?:")
                #print(set_pos_err_code)
if __name__ == "__main__":
    main()