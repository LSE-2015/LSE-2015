#!/usr/bin/env python

import serial
import socket
import time
import sys
import picamera
import wiringpi2 as wiringpi

if __name__ == '__main__':

    delta = 5
    xPos  = 64
    yPos  = 64

   # use BCM GPIO numbers
   wiringpi.wiringPiSetupGpio()

   # Configure interrupt pin
   wiringpi.pinMode(29, OUTPUT)
   wiringpi.digitalWrite(29, LOW)

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

    camera = picamera.PiCamera()
    camera.resolution = (640, 480)
    camera.start_preview()

    while True:
        cmd = ser.read(1)

        d = ((ord(cmd[0]) & (1 << 0)) >> 0)
        u = ((ord(cmd[0]) & (1 << 1)) >> 1)
        r = ((ord(cmd[0]) & (1 << 2)) >> 2)
        l = ((ord(cmd[0]) & (1 << 3)) >> 3)

        if ((d == 0) and (u == 0) and (l == 0) and (r == 0)):
            wiringpi.digitalWrite(29, HIGH)
            sleep 0.05
            wiringpi.digitalWrite(29, LOW)
        elif (((d == 0) and (u == 0)) or ((l == 0) and (r == 0))):
            camera.capture('/home/pi/capture.jpg', use_video_port=True)
        else:
            if (d == 0): xPos = (xPos - delta) if ((xPos - delta) > 0)   else 0
            if (u == 0): xPos = (xPos + delta) if ((xPos + delta) < 127) else 127
            if (r == 0): yPos = (yPos - delta) if ((yPos - delta) > 0)   else 0
            if (l == 0): yPos = (yPos + delta) if ((yPos + delta) < 127) else 127

            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.connect(('', 2301))
            except socket.error, (val, msg):
                print >> sys.stderr, "Could not open socket:\n" + msg
                sys.exit(1)
            s.send(chr(xPos + 0))
            s.shutdown(socket.SHUT_RDWR)
            s.close()

            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.connect(('', 2301))
            except socket.error, (val, msg):
                print >> sys.stderr, "Could not open socket:\n" + msg
                sys.exit(1)
            s.send(chr(yPos + 128))
            s.shutdown(socket.SHUT_RDWR)
            s.close()
