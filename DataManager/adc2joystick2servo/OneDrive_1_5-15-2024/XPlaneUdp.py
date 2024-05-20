import socket
import struct
from time import sleep

class XPlaneUdp():
    """
    UdpSetUp
    WriteDataRef
    WriteCommand
    """

    UDP_IP_XPLANE = '192.168.10.100'
    UDP_IP_XPLANE = '127.0.0.1'
    UDP_PORT_XPLANE_TX = 49000
    UDP_PORT_XPLANE_RX = 49001

    def __init__(self):
        # Open UDP Socket to Transmit
        self.txSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        # ttl = struct.pack('b', 5)
        # txSock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)
        self.rxSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.rxSock.bind((self.UDP_IP_XPLANE, self.UDP_PORT_XPLANE_RX))

    def writeCommand(self, cmdPath):
        msg = struct.pack('=5s500s', b'CMND0', cmdPath.encode('UTF-8'))
        self.txSock.sendto(msg, (self.UDP_IP_XPLANE, self.UDP_PORT_XPLANE_TX))
        # TODO: check for ack?

    def writeDataRef(self, drefPath, val):
        """
        Write Dataref to XPlane
        DREF0+(4byte byte value)+dref_path+0+spaces to complete the whole message to 509 bytes
        DREF0+(4byte byte value of 1)+ sim/cockpit/switches/anti_ice_surf_heat_left+0+spaces to complete to 509 bytes
        """

        cmd = b"DREF\x00"
        dRef = drefPath + "\x00"
        dRefStr = dRef.ljust(500).encode()

        structFmt = None
        if type(val) == bool:
            structFmt = '<5sI500s'
        elif type(val) == float:
            structFmt = '<5sf500s'
        elif type(val) == int:
            structFmt = '<5si500s'
        else:
            print('Value type not defined')
            pass

        msg = struct.pack(structFmt, cmd, val, dRefStr)
        self.txSock.sendto(msg, (self.UDP_IP_XPLANE, self.UDP_PORT_XPLANE_TX))

    def getDataRef(self, dref):
        """ Get RREF from Xplane"""
        cmd = b"RREF"
        fmt = "<4sxii400s"
        freq = 1
        index = 0
        request = struct.pack(fmt, cmd, freq, index, dref)

        self.txSock.sendto(request, (self.UDP_IP_XPLANE, self.UDP_PORT_XPLANE_TX))

        data, addr = self.txSock.recvfrom(4096)
        idx, val = struct.unpack("<if", data[5:13])
        # print(data)

        freq = 0
        msg = struct.pack("<4sxii400s", cmd, freq, index, dref)
        self.txSock.sendto(msg, (self.UDP_IP_XPLANE, self.UDP_PORT_XPLANE_TX))
        return val

    def setupJoystick(self):
        msgPath = 'sim/operation/override_joystick_pitch'
        self.writeDataRef(msgPath, True)
        msgPath = 'sim/operation/override_joystick_roll'
        self.writeDataRef(msgPath, True)
        msgPath = 'sim/operation/override/override_throttles'
        self.writeDataRef(msgPath, True)

    def throttleUp(self, throttle):
        # if throttle < 1:
        throttleCmd = throttle + 0.1
        self.writeDataRef('sim/flightmodel/engine/ENGN_thro[0]', throttleCmd)
        return throttleCmd

    def throttleDown(self, throttle):
        throttleCmd = throttle - 0.1
        self.writeDataRef('sim/flightmodel/engine/ENGN_thro[0]', throttleCmd)
        return throttleCmd

    def pitchTrim(self, trim, val):
        pitchTrimCmd = trim + val
        self.writeDataRef('sim/flightmodel/controls/elv_trim', pitchTrimCmd)
        return pitchTrimCmd

    def rollTrim(self, trim, val):
        rollTrimCmd = trim + val
        self.writeDataRef('sim/flightmodel/controls/ail_trim', rollTrimCmd)
        return rollTrimCmd


def main():
    xplane = XPlaneUdp()
    path = 'sim/operation/override_joystick_pitch'
    xplane.writeDataRef(path, val=True)
    path = 'sim/joystick/yoke_pitch_ratio'
    xplane.writeDataRef(path, val=3.0)

    count = 0
    while count < 10:
        xplane.writeCommand('sim/engines/throttle_up_1')
        count += 1


def takeScreenshot():
    xplane = XPlaneUdp()
    path = "sim/operation/screenshot"
    xplane.writeCommand(path)


def demo_engines():
    xplane = XPlaneUdp()
    path6 = 'sim/operation/override/override_throttles'
    path7 = 'sim/flightmodel/engine/ENGN_thro[0]'
    xplane.writeDataRef(path6, val=True)
    xplane.writeDataRef(path7, val=0.1)


def demo_getDataRef():
    xplane = XPlaneUdp()
    data = xplane.getDataRef(b"sim/flightmodel/misc/act_frc_ptch_lb")
    # data = xplane.demo_getDataRef('sim/aircraft/engines/acf_num_engines')
    # print(data)


if __name__ == '__main__':
    # main()
    # demo_engines()
    # demo_getDataRef()
    while True:
        takeScreenshot()
        sleep(1)