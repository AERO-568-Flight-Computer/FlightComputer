import zmq
from partitonManagerFunc import initialize

def main():
    TIMEOUT = 10000
    FILENAME = "LOG_from_zmqlogger.binlog"
    DELIMBYTES = b'delim_123'
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
            msg = logger_rx_sock.recv()
            logentry = msg + DELIMBYTES
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