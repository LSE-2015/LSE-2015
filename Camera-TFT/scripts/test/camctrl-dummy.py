#!/usr/bin/env  python

import serial
import sys

def isBin(x):
    try:
        a = int(x,base=2)
    except ValueError:
        return False
    else:
        return True

if __name__ == '__main__':

    # Parse arguments
    argNum = len(sys.argv)
    if (argNum !=  5):
        print >> sys.stderr,"Invalid number of arguments ("\
            + str(argNum - 1) + ")."
        print >> sys.stderr,"Should be 4: UP, DOWN, LEFT, RIGHT"
        sys.exit(1)

    if not isBin(sys.argv[1]):
        print >> sys.stderr, "Invalid value (must be binary)"
        sys.exit(1)
    else:
        up = int(sys.argv[1])

    if not isBin(sys.argv[2]):
        print >> sys.stderr, "Invalid value (must be binary)"
        sys.exit(1)
    else:
        down = int(sys.argv[2])

    if not isBin(sys.argv[3]):
        print >> sys.stderr, "Invalid value (must be binary)"
        sys.exit(1)
    else:
        left = int(sys.argv[3])

    if not isBin(sys.argv[4]):
        print >> sys.stderr, "Invalid value (must be binary)"
        sys.exit(1)
    else:
        right = int(sys.argv[4])

    print "UP:%d DOWN:%d LEFT:%d RIGHT:%d" % (up, down, left, right)

    cmd = 0
    cmd = (cmd | (down  << 0))
    cmd = (cmd | (up    << 1))
    cmd = (cmd | (right << 2))
    cmd = (cmd | (left  << 3))

    # connect to serial port
    ser = serial.Serial()
    ser.port     = '/dev/ttyUSB0'
    ser.baudrate = 9600
    ser.bytesize = serial.EIGHTBITS
    ser.stopbits = serial.STOPBITS_ONE
    ser.parity   = serial.PARITY_NONE
    ser.rtscts   = False
    ser.xonxoff  = False
    ser.timeout  = None

    try:
        ser.open()
    except serial.SerialException, msg:
        print >> sys.stderr, "Could not open serial port:\n" + msg
        sys.exit(1)

    ser.write(chr(cmd))
    ser.close()
