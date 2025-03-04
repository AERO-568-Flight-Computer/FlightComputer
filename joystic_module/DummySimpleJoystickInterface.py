import time
import struct
import random
class SimpleJoystickInterface():
    def __init__(self,run_self_calibration = True):
        self.ngi = ""
        self.get_pitch_roll_timeout = 1.0/6.0
        if run_self_calibration:
            time.sleep(4)
        """ ACTIVATION """
        """ ADJUST CALIBRATION FORCE OFFSET """
        time.sleep(4)

    def get_pitch_roll(self):
        pitchPosition = -20.0 + random.random()*40.0
        rollPosition  = -20.0 + random.random()*40.0
        return pitchPosition, rollPosition
    
    def adjustForce(self, ias):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule
        time.sleep(0.01)
    
    @staticmethod
    def adjustForce_old(ngi, axis, ias):
        time.sleep(0.01)

    @staticmethod        
    def __decodeMsg10_partmanager(msg):
        #This is the method from partition manager, the one from current Stirling Inceptor breaks for some reason
        # TODO: make this self.msg10.msgId, etc?
        msgId = msg[0]
        axis = msg[1]
        inceptorNumber = msg[2]
        # status = struct.unpack("L", msg[4:8])  # TODO: further unpack each bit
        # status = struct.unpack("I", msg[4:8])  # Assuming status is a 4-byte unsigned integer
        pos = struct.unpack("f", msg[8:12])
        force = struct.unpack("f", msg[12:16])
        motorDemand = struct.unpack("f", msg[16:20])
        # switchState1 = struct.unpack("L", msg[20:24])
        switch09 = (msg[21] >> 0) & 1  # switch left
        switch10 = (msg[21] >> 1) & 1  # switch forward
        switch11 = (msg[21] >> 2) & 1  # switch right
        switch12 = (msg[21] >> 3) & 1  # switch back
        # switchState2 = struct.unpack("L", msg[24:28])
        analogueSwitch1 = struct.unpack("f", msg[28:32])
        analogueSwitch2 = struct.unpack("f", msg[32:36])
        analogueSwitch3 = struct.unpack("f", msg[36:40])
        ver = struct.unpack("f", msg[40:44])
        rawForceSensorOut = struct.unpack("f", msg[44:48])
        return axis, pos, force, switch09, switch10, switch11, switch12

    @staticmethod
    def __calcForce(airspeed):
        if airspeed < 5:
            airspeed = 5
        return airspeed / 4
    
def main():
    JoysticInteface = SimpleJoystickInterface()
    time.sleep(1)
    #Trying to print out positions every half second
    t_delay = 0.5
    count = 0
    while True:
        pitchPosition, rollPosition = JoysticInteface.get_pitch_roll()
        print("----------------------")
        print("Pitch:", pitchPosition,' idkunits')
        print("Roll:", rollPosition,' idkunits')
        print(count)
        count = count+1
        if count > 20:
            print("Trying to adjust the force")
            JoysticInteface.adjustForce(20)
            count = 0
if __name__ == "__main__":
    main()
