import socket
import struct
from time import sleep
# import csv
# from datetime import datetime, timezone
# import os


class StirlingInceptor():
    UDP_IP_NGI = "192.168.10.101"
    UDP_PORT_INIT = 7000        # Initialization Message
    UDP_PORT_CTL = 7001         # Control Message
    UDP_PORT_ROTCHAR = 7002     # Rotary Characteristic Message
    UDP_PORT_CTLCFG = 7003      # Friction, Model, Shaker, Force Bias, Pos Offset, Linkage Control Message
    UDP_PORT_STATUS = 7004      # Status Message
    UDP_PORT_LIMROT = 7005      # Limited Rotary Characteristic

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

        # TODO: not working i  self.rxSockStatus.bind(('', self.UDP_PORT_STATUS))f NGI is not on network - "only one usage of each socket address is normally permitted

        self.rxSockLimRot = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.rxSockLimRot.bind(('', self.UDP_PORT_LIMROT))
        # TODO: not working if NGI is not on network - "only one usage of each socket address is normally permitted

        """" INITIALIZATION HANDSHAKE """
        self.txSock.sendto(self.msg00(axis='pitch'), (self.UDP_IP_NGI, self.UDP_PORT_INIT))
        self.txSock.sendto(self.msg00(axis='roll'), (self.UDP_IP_NGI, self.UDP_PORT_INIT))

    def forceOffsetLoop(self):
        """
        Feedback loop to get and apply correct pitch and roll offset during initial setup of stick
        Note that this is only necessary because the stick's cal appears to change inconsistently. Assume the stick
        has malfunctioned. :(
        """
        ROLL_CAL_COMPLETE = 0
        PITCH_CAL_COMPLETE = 0
        OFFSET_TOL = 0.5
        ADJUST_VAL = 0.25

        print("Beginning force offset calibration")

        while not ROLL_CAL_COMPLETE and not PITCH_CAL_COMPLETE:
            # Get current stick pitch and roll position and force
            data, addr = self.rxSockStatus.recvfrom(4096)
            axis, pos, force, sw09, sw10, sw11, sw12 = self.decodeMsg10(data)

            # Calculate delta from 0. If delta < 0, the force offset needs to become more positive
            if axis == 0 and not PITCH_CAL_COMPLETE:   # pitch axis TODO: add extra check for position is close to 0
                pitch = pos[0]
                pitchForce = force[0]
                if pitchForce >= OFFSET_TOL:
                    self.PITCH_FORCE_BIAS -= ADJUST_VAL
                    self.txSock.sendto(self.msg07(axis='pitch', forceBias=self.PITCH_FORCE_BIAS),
                                       (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))  # Force Bias Ctrl Msg

                    print(f"pitch force: {pitchForce} | bias decrease | force bias: {self.PITCH_FORCE_BIAS} | position: {pos[0]}")
                elif pitchForce < -OFFSET_TOL:
                    self.PITCH_FORCE_BIAS += ADJUST_VAL
                    self.txSock.sendto(self.msg07(axis='pitch', forceBias=self.PITCH_FORCE_BIAS),
                                       (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))  # Force Bias Ctrl Msg

                    print(f"pitch force: {pitchForce} | bias increase | force bias: {self.PITCH_FORCE_BIAS} | position: {pos[0]}")
                else:
                    PITCH_CAL_COMPLETE = 1
            elif axis == 1 and not ROLL_CAL_COMPLETE:  # roll axis
                roll = pos[0]
                rollForce = force[0]
                if rollForce >= OFFSET_TOL:
                    self.ROLL_FORCE_BIAS -= ADJUST_VAL
                    self.txSock.sendto(self.msg07(axis='roll', forceBias=self.ROLL_FORCE_BIAS),
                                       (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))  # Force Bias Ctrl Msg
                    print(f"roll force: {rollForce} | bias decrease | force bias: {self.ROLL_FORCE_BIAS} | position: {pos[0]}")

                elif rollForce < -OFFSET_TOL:
                    self.ROLL_FORCE_BIAS += ADJUST_VAL
                    self.txSock.sendto(self.msg07(axis='roll', forceBias=self.ROLL_FORCE_BIAS),
                                       (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))  # Force Bias Ctrl Msg
                    print(f"roll force: {rollForce} | bias increase | force bias: {self.ROLL_FORCE_BIAS} | position: {pos[0]}")

                else:
                    ROLL_CAL_COMPLETE = 1

    def IBIT(self):
        sleep(1)
        print("Set joystick to passive")
        msg = self.msg01(axis='pitch', ISUMode=0)
        self.txSock.sendto(msg, (self.UDP_IP_NGI, self.UDP_PORT_CTL))
        sleep(1)
        msg = self.msg01(axis='roll', ISUMode=0)
        self.txSock.sendto(msg, (self.UDP_IP_NGI, self.UDP_PORT_CTL))
        sleep(1)

        print("IBIT")
        msgIBIT = self.msg01(IBIT=True)
        self.txSock.sendto(msgIBIT, (self.UDP_IP_NGI, self.UDP_PORT_CTL))

        sleep(1)

        msgIBIT = self.msg01(IBIT=False)
        self.txSock.sendto(msgIBIT, (self.UDP_IP_NGI, self.UDP_PORT_CTL))

        sleep(30)   # wait for IBIT to be done before allowing script to move on. TODO: check if I actually need this

    def activate(self):
        # Put both pitch and roll axes into active mode
        print("Activating pitch axis")
        msg = self.msg01(axis='pitch', ISUMode=1)
        self.txSock.sendto(msg, (self.UDP_IP_NGI, self.UDP_PORT_CTL))
        sleep(1)

        print("Activating roll axis")
        msg = self.msg01(axis='roll', ISUMode=1)
        self.txSock.sendto(msg, (self.UDP_IP_NGI, self.UDP_PORT_CTL))
        sleep(1)

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

    """ 
    CONTROL MESSAGES - HC TO NGI 
    MSG ID 0:   Initialization Message              Port 7000
    MSG ID 1:   Control Message                     Port 7001
    MSG ID 2:   Rotary Characteristic Message       Port 7002
    MSG ID 5:   Model Control Message               Port 7003
    MSG ID 6:   Stick Shaker Control Message        Port 7003
    MSG ID 7:   Force Bias Control Message          Port 7003
    MSG ID 8:   Position Offset Control Message     Port 7003
    
    """
    def msg00(self, axis):
        """
        Initialization Message
        Byte 1: Message ID - 0
        """
        ba = bytearray([0])                     # byte 1
        ba = ba + self.assignAxis(axis)
        ba = ba + b"\x00\x00"                   # byte 3, 4

        if self.UNITS.lower() == "imperial":
            ba = ba + b"\x01"                   # byte 5
        else:  # if self.UNITS.lower() == "metric":
            ba = ba + b"\x00"                   # byte 5

        ba = ba + bytearray([0, 0, 0])          # byte 6, 7, 8 #TODO: figure out what configuration is
        ba = ba + self.float2byte(self.FREQ_KEEPALIVE)  # bytes 9 - 12
        ba = ba + self.float2byte(self.FREQ_STATUS)     # bytes 13 - 16
        ba = ba + self.float2byte(self.PITCH_GAIN)      # bytes 17 - 20
        ba = ba + self.float2byte(self.PITCH_OFFSET)    # bytes 21 - 24
        ba = ba + self.float2byte(self.ROLL_GAIN)       # bytes 25 - 28
        ba = ba + self.float2byte(self.ROLL_OFFSET)     # bytes 29 - 32
        ba = ba + bytearray(struct.pack("L", 1))        # bytes 33 - 36: Inceptor Number

        return ba

    def msg01(self, axis='pitch', ISUMode=0, IBIT=False, trimSet=False, trimRelease=False,
              beepTrimPlus=False, beepTrimMinus=False, reset=False, stickShake=False):
        """
        Control Message, Message ID: 1
        ISUMode: 0 = Passive, 1 = Active, 2 = Locked
        """
        ba = bytearray([1])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])             # byte 3, 4

        ba = ba + ISUMode.to_bytes(1, 'little')
        ba = ba + bytearray([0])  # byte 6

        ## Byte 7
        byte7 = 0
        ibitBit = 1
        trimSetBit = 1 << 4
        trimReleaseBit = 1 << 5
        beepTrimPlusBit = 1 << 6
        beepTrimMinusBit = 1 << 7

        if IBIT:
            byte7 = byte7 | ibitBit
        if trimSet:
            byte7 = byte7 | trimSetBit
        if trimRelease:
            byte7 = byte7 | trimReleaseBit
        if beepTrimPlus:
            byte7 = byte7 | beepTrimPlusBit
        if beepTrimMinus:
            byte7 = byte7 | beepTrimMinusBit

        byte7 = byte7.to_bytes(1, 'little')
        ba = ba + byte7

        resetBit = 1
        stickShakeBit = 1 << 1
        byte8 = 0

        if reset:
            byte8 = byte8 | resetBit
        if stickShake:
            byte8 = byte8 | stickShakeBit

        ba = ba + byte8.to_bytes(1, 'little')

        ba = ba + bytearray([0, 0, 0, 0])       # byte 9 - 12

        #TODO: there are 4 extra bytes showing up in wireshark - have a question out to Stirling about it
        ba = ba + bytearray([0, 0, 0, 0])       # bytes 13 - 16
        return ba

    def msg02(self, PosCoords, NegCoords, axis='pitch'):
        """
        Rotary Characteristic Message - PORT 7002
        PosCoords: [[Position 1, Force 1], [Position 2, Force 2], ... [Position 5, Force 5]]
        NegCoords: [[Position 1, Force 1], [Position 2, Force 2], ... [Position 5, Force 5]]
        """
        ba = bytearray([2])  # byte 1
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0, 0, 0, 0, 0])       # bytes 3 - 8

        for PosForcePair in PosCoords:
            ba = ba + self.float2byte(PosForcePair[0])
            ba = ba + self.float2byte(PosForcePair[1])

        for PosForcePair in NegCoords:
            ba = ba + self.float2byte(PosForcePair[0])
            ba = ba + self.float2byte(PosForcePair[1])

        return ba

    def msg05(self, axis='pitch', mass=0.008999998681247234, damping=0.8991907238960266):
        """
        Msg 05: Model Control Message
        """
        ba = bytearray([5])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(mass)
        ba = ba + self.float2byte(damping)

        return ba

    def msg06(self, axis='pitch', shakeFreq=1.00, shakeAmp=0.0):
        """
        Msg 06: Shaker Control Message
        """
        ba = bytearray([6])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(shakeFreq)
        ba = ba + self.float2byte(shakeAmp)

        return ba

    def msg07(self, axis='pitch', forceBias=0.0):
        """
        Msg 07: Force Bias Control Message
        """
        ba = bytearray([7])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(forceBias)

        return ba

    def msg08(self, axis='pitch', posOff=0.0):
        """
        Msg 08: Position Offset Control Message
        """
        ba = bytearray([7])
        ba = ba + self.assignAxis(axis)
        ba = ba + bytearray([0, 0])
        ba = ba + self.float2byte(posOff)

        return ba

    """
    STATUS MESSAGES - NGI TO HC
    MSG ID 10: Status Message                   Port 7004
    MSG ID 11: Limited Rotary Characteristic    Port 7005
    """
    def decodeMsg10(self, msg):
        # TODO: make this self.msg10.msgId, etc?
        msgId = msg[0]
        axis = msg[1]
        inceptorNumber = msg[2]
        status = struct.unpack("L", msg[4:8])   #TODO: further unpack each bit
        pos = struct.unpack("f", msg[8:12])
        force = struct.unpack("f", msg[12:16])
        motorDemand = struct.unpack("f", msg[16:20])
        switchState1 = struct.unpack("L", msg[20:24])
        switch09 = (msg[21] >> 0) & 1   # switch left
        switch10 = (msg[21] >> 1) & 1   # switch forward
        switch11 = (msg[21] >> 2) & 1   # switch right
        switch12 = (msg[21] >> 3) & 1   # switch back
        switchState2 = struct.unpack("L", msg[24:28])
        analogueSwitch1 = struct.unpack("f", msg[28:32])
        analogueSwitch2 = struct.unpack("f", msg[32:36])
        analogueSwitch3 = struct.unpack("f", msg[36:40])
        ver = struct.unpack("f", msg[40:44])
        rawForceSensorOut = struct.unpack("f", msg[44:48])

        return axis, pos, force, switch09, switch10, switch11, switch12

    def decodeMsg11(self, msg):
        msgId = msg[0]
        axis = msg[1]
        inceptorNumber = msg[2]

        [posCoord1Pos] = struct.unpack("f", msg[4:8])
        [posCoord1Force] = struct.unpack("f", msg[8:12])
        [posCoord2Pos] = struct.unpack("f", msg[12:16])
        [posCoord2Force] = struct.unpack("f", msg[16:20])
        [posCoord3Pos] = struct.unpack("f", msg[20:24])
        [posCoord3Force] = struct.unpack("f", msg[24:28])
        [posCoord4Pos] = struct.unpack("f", msg[28:32])
        [posCoord4Force] = struct.unpack("f", msg[32:36])
        [posCoord5Pos] = struct.unpack("f", msg[36:40])
        [posCoord5Force] = struct.unpack("f", msg[40:44])
        [posCoord6Pos] = struct.unpack("f", msg[44:48])
        [posCoord6Force] = struct.unpack("f", msg[48:52])
        posCoords = [[posCoord1Pos, posCoord1Force],
                     [posCoord2Pos, posCoord2Force],
                     [posCoord3Pos, posCoord3Force],
                     [posCoord4Pos, posCoord4Force],
                     [posCoord5Pos, posCoord5Force],
                     [posCoord6Pos, posCoord6Force]]

        [negCoord1Pos] = struct.unpack("f", msg[52:56])
        [negCoord1Force] = struct.unpack("f", msg[56:60])
        [negCoord2Pos] = struct.unpack("f", msg[60:64])
        [negCoord2Force] = struct.unpack("f", msg[64:68])
        [negCoord3Pos] = struct.unpack("f", msg[68:72])
        [negCoord3Force] = struct.unpack("f", msg[72:76])
        [negCoord4Pos] = struct.unpack("f", msg[76:80])
        [negCoord4Force] = struct.unpack("f", msg[80:84])
        [negCoord5Pos] = struct.unpack("f", msg[84:88])
        [negCoord5Force] = struct.unpack("f", msg[88:92])
        [negCoord6Pos] = struct.unpack("f", msg[92:96])
        [negCoord6Force] = struct.unpack("f", msg[96:100])
        negCoords = [[negCoord1Pos, negCoord1Force],
                     [negCoord2Pos, negCoord2Force],
                     [negCoord3Pos, negCoord3Force],
                     [negCoord4Pos, negCoord4Force],
                     [negCoord5Pos, negCoord5Force],
                     [negCoord6Pos, negCoord6Force]]

        return axis, posCoords, negCoords

    def tearDown(self):
        print("Tearing down open sockets.")
        self.txSock.close()
        self.rxSockStatus.close()

    def configSetup(self):
        print("Setting up configuration of stick")
        """ PITCH AXIS CONFIGURATION """
        self.txSock.sendto(self.msg08(axis='pitch'), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))   # Position Offset Ctrl Msg
        self.txSock.sendto(self.msg07(axis='pitch', forceBias=self.PITCH_FORCE_BIAS), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))   # Force Bias Ctrl Msg
        self.txSock.sendto(self.msg06(axis='pitch'), (self.UDP_IP_NGI, self.UDP_PORT_INIT))     # Stick Control Shaker Msg
        self.txSock.sendto(self.msg05(axis='pitch'), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))   # Model Ctrl Msg
        # self.txSock.sendto(self.msg02(self.POS_FORCE_COORDS, self.NEG_FORCE_COORDS, axis='pitch'), (self.UDP_IP_NGI, self.UDP_PORT_ROTCHAR))

        """ ROLL AXIS CONFIGURATION """
        self.txSock.sendto(self.msg08(axis='roll'), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))
        self.txSock.sendto(self.msg07(axis='roll', forceBias=self.ROLL_FORCE_BIAS), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))
        self.txSock.sendto(self.msg06(axis='roll'), (self.UDP_IP_NGI, self.UDP_PORT_INIT))
        self.txSock.sendto(self.msg05(axis='roll'), (self.UDP_IP_NGI, self.UDP_PORT_CTLCFG))
        # self.txSock.sendto(self.msg02(self.POS_FORCE_COORDS, self.NEG_FORCE_COORDS, axis='roll'), (self.UDP_IP_NGI, self.UDP_PORT_ROTCHAR))


def main():
    ngi = StirlingInceptor()
    try:

        """ IBIT """
        ngi.IBIT()

        """ ACTIVATION """
        ngi.activate()

        ngi.configSetup()

        while True:
            print("test")
            data = ngi.rxSockLimRot.recvfrom(1)
            print("data received: ")
            axis, posCoords, negCoords = ngi.decodeMsg11(data)
            print(f"Positive {axis} Pos/Force Coordinates: {posCoords} | Negative Pos/Force Coordinates: {negCoords}")
            ngi.POS_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 40]]
            ngi.NEG_FORCE_COORDS = [[0, 0], [5, 5], [10, 10], [15, 15], [20, 40]]
            # ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis=axis), (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))
            ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis='pitch'),
                              (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))
            ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis='roll'),
                              (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))

            data, addr = ngi.rxSockLimRot.recvfrom(4096)
            axis, posCoords, negCoords = ngi.decodeMsg11(data)
            print(f"Positive {axis} Pos/Force Coordinates: {posCoords} | Negative Pos/Force Coordinates: {negCoords}")
    except KeyboardInterrupt as e:
        print(e)
    finally:
        ngi.tearDown()


if __name__ == "__main__":
    main()