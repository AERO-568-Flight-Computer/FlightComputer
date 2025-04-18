import os
class Msg10LogParser:
    def __init__(self,log_filename):
        self.log_filename = log_filename
        self.startbytes = b'startmsg'
        self.endbytes   = b'endmsg'

    def parseFile(self):
        #Loading the whole file
        logfile = open(self.log_filename,'br')
        binstr = logfile.read()

        #Separating into messages
        len_end = len(self.endbytes)
        messages = binstr.split(self.startbytes)
        vallid_msg_10s = []
        i = 0
        for message in messages:
            #If it actually has the right length... I neeed to check a lot of cases other than nominal....
            if message[-len_end:] == self.endbytes:
                valid_msg_10 = message[:-len_end]
                vallid_msg_10s.append(valid_msg_10)
                i  = i+1
        return vallid_msg_10s
    
    def print_msg_10s(self,valid_msg_10s):
        for msg in valid_msg_10s:
            print(msg.hex('-'))
    def print_msg_10_switch_state(self,valid_msg_10s):
        for msg in valid_msg_10s:
            print(msg[20:28].hex('-'))

def main():
    Parser = Msg10LogParser("joystic_raw_log.binlog")
    files = os.listdir()
    print(files)
    valid_msg_10s = Parser.parseFile()
    Parser.print_msg_10s(valid_msg_10s)
    Parser.print_msg_10_switch_state(valid_msg_10s)
if __name__ == '__main__':
    main()