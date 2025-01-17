from .joystic_related.NGIcalibration import StirlingInceptor
import time
class SimpleJoysticInteface():
    def __init__(self,run_self_calibration = True):
        self.ngi = StirlingInceptor()
        self.get_pitch_roll_timeout = 1/6
        if run_self_calibration:
            self.ngi.IBIT()

    def get_pitch_roll(self):
        #So there is message 11 which should have rotary characteristics?
        #But everything uses message 10 to get pitch and roll.
        #So just get 4096 bytes from the apppropriate socket, and decode message 10 untill have pitch and roll.
        #Timeout if time passsed is more than timeout, return a fail.
        time_start = time()
        time_now = time_start
        runtime = 0
        pitch_found = False
        roll_found = False
        while (runtime < self.get_pitch_roll_timeout) :
            data, addr = self.ngi.rxSockStatus.recvfrom(4096)
            axis, pos, force, trimlft, trimup, trimrht, trimdwn = self.ngi.decodeMsg10(data)
            time_now = time()
            runtime = time_now - time_start
            if axis == 0:
                #pitch axis
                pitchPosition = pos[0]
                pitch_found = True
            if axis == 1:
                #roll axis
                rollPosition = pos[0]
                roll_found = True
            if (roll_found and pitch_found):
                return pitchPosition, rollPosition, 0
        print("Couldn't get Joystic location in time")
        return -1,-1,-1
    
    def adjustForce(self, ias):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule
        if ias < 5:
            ias = 5
        force = SimpleJoysticInteface.__calcForce(ias)

        for axis in ['pitch','roll']:
            if axis == 'pitch':
                scale = 1.5
            else:
                scale = 1

            # print(f"ias: {ias} | force: {force}")

            self.ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
            self.ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
            self.ngi.txSock.sendto(self.ngi.msg02(self.ngi.POS_FORCE_COORDS, self.ngi.NEG_FORCE_COORDS, axis),
                            (self.ngi.UDP_IP_NGI, self.ngi.UDP_PORT_ROTCHAR))
        
    @staticmethod
    def __calcForce(airspeed):
        if airspeed < 5:
            airspeed = 5
        return airspeed / 4
