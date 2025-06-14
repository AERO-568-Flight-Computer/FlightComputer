import zmq
import os
from partitonManagerFunc import initialize
import time
import struct

def main():
    #Trying to make a folder to store logs in
    time_and_date = time.asctime()
    time_and_date = time_and_date.replace(' ','_')
    time_and_date = time_and_date.replace(':','_')

    DIRNAME = "logs" + "/" + "opa_log_" + time_and_date
    try:
        os.makedirs(DIRNAME)
    except Exception as e:
        print(e)
        raise

    TIMEOUT = 10000
    FILENAME = DIRNAME + '/' + "opa_log_" + time_and_date + ".binlog" #Going to generate filename based on time and date.
    #Than adjust filename in the logger 
    DELIMBYTES = b'delim_123'
    DELIMTIME  = b'delt'
    SESSIONSTART = b'SESSIONSTART'
    REOPEN_ITER = 1000

    context = zmq.Context()
    logger_rx_sock = context.socket(zmq.PULL)
    logger_rx_sock.setsockopt(zmq.RCVTIMEO, TIMEOUT)
    logger_rx_sock.setsockopt(zmq.LINGER, 0)
    logger_rx_sock.setsockopt(zmq.CONFLATE, 1)
    logger_rx_sock.connect('tcp://localhost:6100')

    logfile = open(FILENAME,'bw')
    logfile.write(SESSIONSTART)

    initialize.initialize()
    print("Initialized")

    try:
        i = 0
        while True:
            time_msg = time.time()
            time_str = struct.pack('d',time_msg)

            msg = logger_rx_sock.recv()
            logentry = msg + DELIMTIME + time_str + DELIMBYTES
            logfile.write(logentry)
            i = i+1
            if i >= REOPEN_ITER:
                print("Reopening")
                logfile.close()
                logfile = open(FILENAME,'ba')
                i = 0
    except Exception as e:
        print(e)
        raise
    finally:
        logger_rx_sock.close()
        context.term()
        logfile.close()



if __name__ == "__main__":
    main()