'''
import socket
import struct
from time import sleep

class StirlingInceptor():
    UDP_IP_NGI = "192.168.10.101"
    UDP_PORT_INIT = 7000  # Initialization Message
    UDP_PORT_CTL = 7001  # Control Message
    UDP_PORT_ROTCHAR = 7002  # Rotary Characteristic Message
    UDP_PORT_CTLCFG = 7003  # Friction, Model, Shaker, Force Bias, Pos Offset, Linkage Control Message
    UDP_PORT_STATUS = 7004  # Status Message
    UDP_PORT_LIMROT = 7005  # Limited Rotary Characteristic

    PITCH_GAIN = -0.0609
    PITCH_OFFSET = -15.1641

    ROLL_GAIN = -0.0583
    ROLL_OFFSET = 21.6876

    PITCH_FORCE_BIAS = 1.5
    ROLL_FORCE_BIAS = 7.0

    UNITS = "metric"
    FREQ_KEEPALIVE = 0.0
    FREQ_STATUS = 20.0
    FREQ_CTL = 0.0
    INCEPTOR_NUMBER = 1

    PITCH_MIN = -20.0
    PITCH_MAX = 20.0
    ROLL_MIN = -20.0
    ROLL_MAX = 20.0

    POS_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 20]]
    NEG_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 20]]

    def __init__(self):
        # Open UDP Socket to Transmit
        ttl = struct.pack('b', 5)
        self.txSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.txSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.txSock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

        # Open UDP Socket to Receive Stick Status
        self.rxSockStatus = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.rxSockStatus.bind(('', self.UDP_PORT_STATUS))

    def float2byte(self, floatVal):
        return bytearray(struct.pack("f", floatVal))

    def assignAxis(self, axis):
        ba = None

        if axis == 0 or axis.lower() == "pitch":
            ba = b"\x00"  # byte 2
        elif axis == 1 or axis.lower() == "roll":
            ba = b"\x01"  # byte 2
        else:
            print("None: No known axis defined")

        return ba

    def msg08(self, axis='pitch', posOff=0.0):
        """
        Msg 08: Position Offset Control Message
        """
        ba = bytearray([8])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(posOff)

        return ba

    def move_joystick_to_position(self, pitch_pos, roll_pos, duration=5, steps=50):
        """
        Moves the joystick to the specified position for both pitch and roll axes gradually over time.
        :param pitch_pos: The desired position for the pitch axis (-20 to 20).
        :param roll_pos: The desired position for the roll axis (-20 to 20).
        :param duration: The duration over which the movement should occur (in seconds).
        :param steps: The number of steps to divide the movement into.
        """
        # Calculate the increments for pitch and roll axes
        pitch_increment = (pitch_pos - self.PITCH_OFFSET) / steps
        roll_increment = (roll_pos - self.ROLL_OFFSET) / steps

        # Send control messages to NGI device for each step
        for step in range(steps):
            # Calculate the current position for pitch and roll axes
            current_pitch = self.PITCH_OFFSET + step * pitch_increment
            current_roll = self.ROLL_OFFSET + step * roll_increment

            # Create control messages for pitch and roll axes
            msg_pitch = self.msg08(axis='pitch', posOff=current_pitch)
            msg_roll = self.msg08(axis='roll', posOff=current_roll)

            # Send control messages to NGI device
            self.txSock.sendto(msg_pitch, (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))
            self.txSock.sendto(msg_roll, (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))

            # Calculate delay between steps
            step_delay = duration / steps
            sleep(step_delay)


def main():
    ngi = StirlingInceptor()
    try:
        # Move joystick to position gradually
        ngi.move_joystick_to_position(pitch_pos=10, roll_pos=-5, duration=10, steps=100)
    except KeyboardInterrupt:
        print("Keyboard interrupt detected. Exiting...")
    finally:
        ngi.rxSockStatus.close()


if __name__ == "__main__":
    main()
'''


import socket
import struct
from time import sleep

class StirlingInceptor():
    UDP_IP_NGI = "192.168.10.101"
    UDP_PORT_INIT = 7000  # Initialization Message
    UDP_PORT_CTL = 7001  # Control Message
    UDP_PORT_ROTCHAR = 7002  # Rotary Characteristic Message
    UDP_PORT_CTLCFG = 7003  # Friction, Model, Shaker, Force Bias, Pos Offset, Linkage Control Message
    UDP_PORT_STATUS = 7004  # Status Message
    UDP_PORT_LIMROT = 7005  # Limited Rotary Characteristic

    PITCH_GAIN = -0.0609
    PITCH_OFFSET = -15.1641

    ROLL_GAIN = -0.0583
    ROLL_OFFSET = 21.6876

    PITCH_FORCE_BIAS = 1.5
    ROLL_FORCE_BIAS = 7.0

    UNITS = "metric"
    FREQ_KEEPALIVE = 0.0
    FREQ_STATUS = 20.0
    FREQ_CTL = 0.0
    INCEPTOR_NUMBER = 1

    PITCH_MIN = -20.0
    PITCH_MAX = 20.0
    ROLL_MIN = -20.0
    ROLL_MAX = 20.0

    POS_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 20]]
    NEG_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 20]]

    def __init__(self):
        # Open UDP Socket to Transmit
        ttl = struct.pack('b', 5)
        self.txSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.txSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.txSock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

        # Open UDP Socket to Receive Stick Status
        self.rxSockStatus = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.rxSockStatus.bind(('', self.UDP_PORT_STATUS))

    def float2byte(self, floatVal):
        return bytearray(struct.pack("f", floatVal))

    def assignAxis(self, axis):
        ba = None

        if axis == 0 or axis.lower() == "pitch":
            ba = b"\x00"  # byte 2
        elif axis == 1 or axis.lower() == "roll":
            ba = b"\x01"  # byte 2
        else:
            print("None: No known axis defined")

        return ba

    def msg08(self, axis='pitch', posOff=0.0):
        """
        Msg 08: Position Offset Control Message
        """
        ba = bytearray([8])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(posOff)

        return ba

    def move_joystick_to_position(self, pitch_pos, roll_pos):
        """
        Moves the joystick to the specified position for both pitch and roll axes.
        :param pitch_pos: The desired position for the pitch axis (-20 to 20).
        :param roll_pos: The desired position for the roll axis (-20 to 20).
        """
        # Create control messages for pitch and roll axes
        msg_pitch = self.msg08(axis='pitch', posOff=pitch_pos)
        msg_roll = self.msg08(axis='roll', posOff=roll_pos)

        # Send control messages to NGI device
        self.txSock.sendto(msg_pitch, (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))
        self.txSock.sendto(msg_roll, (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))


def main():
    ngi = StirlingInceptor()
    try:
        # Move joystick to position
        ngi.move_joystick_to_position(pitch_pos=10, roll_pos=-5)
    except KeyboardInterrupt:
        print("Keyboard interrupt detected. Exiting...")
    finally:
        ngi.rxSockStatus.close()


if __name__ == "__main__":
    main()
