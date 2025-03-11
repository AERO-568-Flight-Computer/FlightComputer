import time
class Servo:
    def __init__(self,port_string,actuator_id = 0x01):
        #port string is the port at which the servo is going to be.
        #'/dev/ttyS4' is the port of the servo we run
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
        time.sleep(0.005)
        print("DesiredPos:",servo_desired_pos_deg)
        return 0
    
    def get_pos(self):
        #Checking power
        time.sleep(0.005)
        pos_deg = 42
        return pos_deg, 0         
