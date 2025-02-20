import time
from opa_msg_library import *
from joystic_module.SimpleJoystickInterface import SimpleJoystickInterface

verbose = True
jsk_id = b'JK'
def main():
    joystic_max_freq = 2
    JoysticInteface = SimpleJoystickInterface()
    sent_failed_count = 0
    while True:
        JoysticInteface.adjustForce(50)
        #Trying to read jostic status
        pitch, roll, _ = JoysticInteface.get_pitch_roll()
        time_now = time.time()
        state_msg = pack_joystic_state_msg(jsk_id,time_now,pitch,roll)
        print(f"Joystic state message: {unpack_joystic_state_msg(state_msg)}")
        time.sleep(1/joystic_max_freq)
if __name__ == '__main__':
    main()