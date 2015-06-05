#!/usr/bin/env python

import serial
import time
import sys

if __name__ == '__main__':

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

    while True:
        ser.flushInput()
        cmd = ser.read(1)

        joy_down  = ((ord(cmd[0]) & (1 << 0)) >> 0)
        joy_up    = ((ord(cmd[0]) & (1 << 1)) >> 1)
        joy_right = ((ord(cmd[0]) & (1 << 2)) >> 2)
        joy_left  = ((ord(cmd[0]) & (1 << 3)) >> 3)

        btn_low   = ((ord(cmd[0]) & (1 << 4)) >> 4)
        btn_high  = ((ord(cmd[0]) & (1 << 5)) >> 5)
        btn_stop  = ((ord(cmd[0]) & (1 << 6)) >> 6)
        btn_dummy = ((ord(cmd[0]) & (1 << 7)) >> 7)

        # Test
        print "joy_up    - %d    joy_down  - %d    joy_left  - %d    joy_right - %d    " \
              "btn_high  - %d    btn_stop  - %d    btn_low   - %d    btn_dummy - %d" \
        % (joy_up, joy_down, joy_left, joy_right, btn_high, btn_stop, btn_low, btn_dummy)

