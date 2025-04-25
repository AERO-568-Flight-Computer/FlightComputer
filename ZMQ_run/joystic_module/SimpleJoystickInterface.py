from NGIcalibration import StirlingInceptor
import time
import struct
class SimpleJoystickInterface():
    def __init__(self,run_self_calibration = True):
        self.ngi = StirlingInceptor()
        self.get_pitch_roll_timeout = 1.0/6.0
        if run_self_calibration:
            self.ngi.IBIT()
        """ ACTIVATION """
        self.ngi.activate()
        """ ADJUST CALIBRATION FORCE OFFSET """
        time.sleep(2)
        self.ngi.configSetup()
        time.sleep(2)
        
        self.log_flag = True        
        if self.log_flag:
            self.log_end_string   = b'endmsg'
            self.log_start_string = b'startmsg' 
            self.log_filename = "joystic_raw_log.binlog"
            self.log_file = open(self.log_filename,"bw")
            self.log_counter = 1000
        else:
            self.log_end_string   = None
            self.log_start_string = None
            self.log_filename = None
            self.log_file = None

    def get_pitch_roll(self):
        #So there is message 11 which should have rotary characteristics?
        #But everything uses message 10 to get pitch and roll.
        #So just get 4096 bytes from the apppropriate socket, and decode message 10 untill have pitch and roll.
        #Timeout if time passsed is more than timeout, return a fail.
        time_start = time.time()
        time_now = time_start
        runtime = 0
        pitch_found = False
        roll_found = False
        while (runtime < self.get_pitch_roll_timeout) :
            data, addr = self.ngi.rxSockStatus.recvfrom(4096)
            axis, pos, force, switch09, switch10, switch11, switch12 = self.__decodeMsg10_partmanager(data)
            #print("------Data manager decode done-----------")
            #axis, pos, force, trimlft, trimup, trimrht, trimdwn = self.ngi.decodeMsg10(data)
            #print("----------Striling inceptor decode done----------")
            time_now = time.time()
            runtime = time_now - time_start
            if axis == 0:
                #pitch axis
                pitchPosition = pos[0]
                pitch_found = True
            if axis == 1:
                #roll axis
                rollPosition = pos[0]
                roll_found = True

            if (pitch_found or roll_found) and False:
                #log data
                print("Logging...")
                if self.log_flag:
                    logmsg = self.log_start_string + data + self.log_end_string
                    self.log_file.write(logmsg)
                    self.log_counter = self.log_counter -1
                    if self.log_counter <= 0:
                        self.log_file.close()
                        print("LOGGING SESSION DONE")
                        #TERMINATING, HOW DO I DO IT BETTER
                        raise("Logging session done")
                    
            if (roll_found and pitch_found):
                return pitchPosition, rollPosition
            
        #print("Couldn't get Joystic location in time")
        raise Exception("Timeout")
    
    def adjustForce(self, ias):
    # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
    # if negative, send the first pos/force coordinate on the neg schedule
        if ias < 5:
            ias = 5
        force = SimpleJoystickInterface.__calcForce(ias)

        for axis in ['pitch','roll']:
            if axis == 'pitch':
                scale = 1.5
            else:
                scale = 1

            #print(f"ias: {ias} | force: {force}")

            #self.ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
            #self.ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
            #self.ngi.txSock.sendto(self.ngi.msg02(self.ngi.POS_FORCE_COORDS, self.ngi.NEG_FORCE_COORDS, axis),
            #                (self.ngi.UDP_IP_NGI, self.ngi.UDP_PORT_ROTCHAR))
            self.adjustForce_old(self.ngi,axis,ias)
    
    @staticmethod
    def adjustForce_old(ngi, axis, ias):
        #print("Trying to run the old method")
            # check deflection on joystick - if positive, send the first pos/force coordinate on the schedule
            # if negative, send the first pos/force coordinate on the neg schedule

        if ias < 5:
            ias = 5
        force = SimpleJoystickInterface.__calcForce(ias)

        if axis == 'pitch':
            scale = 1.5
        else:
            scale = 1

        # print(f"ias: {ias} | force: {force}")

        ngi.POS_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
        ngi.NEG_FORCE_COORDS = [[0, 0], [5, scale*force], [10, 1.25*scale*force], [15, 1.5*scale*force], [20, 1.75*scale*force]]
        ngi.txSock.sendto(ngi.msg02(ngi.POS_FORCE_COORDS, ngi.NEG_FORCE_COORDS, axis),
                          (ngi.UDP_IP_NGI, ngi.UDP_PORT_ROTCHAR))

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
    t_delay = 0.05
    count = 0
    while True:
        pitchPosition, rollPosition = JoysticInteface.get_pitch_roll()
        print("----------------------")
        print("Pitch:", pitchPosition,' idkunits')
        print("Roll:", rollPosition,' idkunits')
        print(count)
        count = count+1
        if count > 20:
            #print("Trying to adjust the force")
            JoysticInteface.adjustForce(20)
            count = 0
        time.sleep(t_delay)
if __name__ == "__main__":
    main()
