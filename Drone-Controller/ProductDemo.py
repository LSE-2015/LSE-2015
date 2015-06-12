#!/usr/bin/python
import time
import sys
import tty
import termios
import argparse
import cflib
import serial
from cflib.crazyflie import Crazyflie
#from cfclient.utils.logconfigreader import LogConfig
#from cfclient.utils.logconfigreader import LogVariable
from threading import Thread, Event
from datetime import datetime
accelvaluesX = []
accelvaluesY = []
accelvaluesZ = []
#import Gnuplot

class TestFlight:

    roll = 0 	
    pitch = 0	
    yawrate = 0	
    thrust = 0

    hold = "False"

    trimmed_roll = 0
    trimmed_pitch = 0

    def __init__(self):
        """
        Initialize the quadcopter
        """
        self.f = open('log.log', 'w')

        self.starttime = time.time()*1000.0

        self.crazyflie = cflib.crazyflie.Crazyflie()
        print 'Initializing drivers' 
        cflib.crtp.init_drivers()
 
        print 'Searching for available devices'
        available = cflib.crtp.scan_interfaces()

        radio = False
        for i in available:
            # Connect to the first device of the type 'radio'
            if 'radio' in i[0]:
                radio = True
                dev = i[0]
                print 'Connecting to interface with URI [{0}] and name {1}'.format(i[0], i[1])
                self.crazyflie.open_link(dev)
                break

        if not radio:
            print 'No quadcopter detected. Try to connect again.'
            exit(-1)

        # Inicialize Serial reading
        # connect to serial port
        self.ser = serial.Serial()
        self.ser.port     = '/dev/ttyUSB0'
        self.ser.baudrate = 9600
        self.ser.bytesize = serial.EIGHTBITS
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.parity   = serial.PARITY_NONE
        self.ser.rtscts   = False
        self.ser.xonxoff  = False
        self.ser.timeout  = None

        try:
            self.ser.open()
        except serial.SerialException, msg:
            print >> sys.stderr, "Could not open serial port\n"
            sys.exit(1) 

        # Set up the callback when connected
        self.crazyflie.connected.add_callback(self.connectSetupFinished)


    def connectSetupFinished(self, linkURI):

        Thread(target=self.increasing_step).start() 
        Thread(target=self.pulse_command).start()

    def print_stab_data(self, ident, data, logconfig):
        #sys.stdout.write('Stabilizer: Roll={1:.2f}, Pitch={2:.2f}\r'.format(data["stabilizer.roll"], data["stabilizer.pitch"]))
        #sys.stdout.flush()

        trim_roll = (-1)*data["stabilizer.roll"] + 3.2
        trim_pitch = (-1)*data["stabilizer.pitch"] -0.2

        if trim_roll != 0 or trim_pitch != 0:    
            self.trimmed_roll = self.roll + trim_roll
            self.trimmed_pitch = self.pitch + trim_pitch

    def increasing_step(self):

        min_thrust = 10000
        max_thrust = 60000
        thrust_increment = 500

        roll_increment = 30
        min_roll = -50
        max_roll = 50

        roll_increment = 30
        min_roll = -50
        max_roll = 50

        pitch_increment = 30
        min_pitch = -50
        max_pitch = 50

        yaw_increment = 30
        min_yaw = -200
        max_yaw = 200

        stop_moving_count = 0

        while 1:
            #command = raw_input("####Press 'e' to exit the game#####")

            #Reading from serial Port.
            self.ser.flushInput()
            cmd = self.ser.read(1)

            joy_backward = ((ord(cmd[0]) & (1 << 0)) >> 0)
            joy_fordward = ((ord(cmd[0]) & (1 << 1)) >> 1)
            joy_right    = ((ord(cmd[0]) & (1 << 2)) >> 2)
            joy_left     = ((ord(cmd[0]) & (1 << 3)) >> 3)

            btn_low   = ((ord(cmd[0]) & (1 << 4)) >> 4)
            btn_high  = ((ord(cmd[0]) & (1 << 5)) >> 5)
            btn_stop  = ((ord(cmd[0]) & (1 << 6)) >> 6)
            btn_start = ((ord(cmd[0]) & (1 << 7)) >> 7)

            #if (command=="e"):
				# Exit the main loop
             #   self.pitch = 0
              #  self.roll = 0
              #  self.thrust = 0
              #  self.yaw = 0
              #  print "Exiting main loop in 1 second"
              #  time.sleep(0.5)
              #  self.crazyflie.close_link() # This errors out for some reason. Bad libusb?

            # Button Panel: 
            if btn_high == 0 and (self.thrust + thrust_increment <= max_thrust):
                self.thrust += thrust_increment
            elif btn_low == 0 and (self.thrust - thrust_increment >= min_thrust):
                self.thrust -= thrust_increment

            # Start/Stop
            if btn_start == 0 and (self.thrust + thrust_increment <= max_thrust):
                self.thrust = 30000
            elif btn_stop == 0 and (self.thrust - thrust_increment >= min_roll):
                self.thrust = 0

            # Joystic: 
            elif joy_fordward == 0 and (self.pitch + pitch_increment <= max_pitch):
                self.pitch += pitch_increment
                stop_moving_count = 0
            elif joy_backward == 0 and (self.pitch - pitch_increment >= min_pitch):
                self.pitch -= pitch_increment
                stop_moving_count = 0
            elif joy_right == 0 and (self.roll + roll_increment <= max_roll):
                self.roll += roll_increment
                stop_moving_count = 0
            elif joy_left == 0 and (self.roll - roll_increment >= min_roll):
                self.roll -= roll_increment
                stop_moving_count = 0

      
            else:
                # The controls are not being touch, get back to zero roll, pitch and yaw
                if stop_moving_count >= 40:
                    self.pitch = 0
                    self.roll = 0
                    self.yaw = 0
                else:
                    stop_moving_count += 1
            
            #assert self.thrust <= USHRT_MAX
            self.crazyflie.commander.send_setpoint(self.roll, self.pitch, self.yawrate, self.thrust)              

    #def pulse_command(self):

     #   while 1:
            #self.crazyflie.param.set_value('flightmode.althold', self.hold)
                
      #      self.crazyflie.commander.send_setpoint(self.roll, self.pitch, self.yawrate, self.thrust)
       #     time.sleep(0.5)       
	 

TestFlight()
