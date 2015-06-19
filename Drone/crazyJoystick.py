#!/usr/bin/python
import time
import sys
import serial
sys.path.append("../lib")
import cflib
import math
import threading
import logging
import cflib.crtp
#import os
#os.environ['SDL_VIDEODRIVER'] = 'dummy'
import pygame
import pygame.locals
from pygame.locals import *
from cflib.crazyflie import Crazyflie
from cfclient.utils.logconfigreader import LogConfig
from cfclient.utils.logconfigreader import LogVariable
from cflib.utils.callbacks import Caller
from threading import Thread, Event
from datetime import datetime


#logging.basicConfig(level=logging.DEBUG,
#                    format='(%(threadName)-10s) %(message)s',
#                    )

# Only output errors from the logging framework
#logging.basicConfig(level=logging.ERROR)

pygame.init()
screen = pygame.display.set_mode((640, 480))
clock = pygame.time.Clock()
pygame.display.set_caption('Pygame Caption')
pygame.mouse.set_visible(0)

roll = 0
pitch = 0
yaw = 0
thrust = 0
trim_roll = 0
trim_pitch = 0
trim_yaw = 0
alive = "True"

#joy_down  = 1
#joy_up    = 1
#joy_right = 1
#joy_left  = 1

#btn_low   = 1
#btn_high  = 1
#btn_stop  = 1
#btn_dummy = 1
#connected = "False"

class TestFlight:

	def __init__(self, link_uri):

		self.control = "False"
		self.connected = "False"
		self._param_check_list = []
		self._param_groups = []
		self.starttime = time.time()*1000.0

		#Initialize and run the example with the specified link_uri
		# Create a Crazyflie object without specifying any cache dirs
		self.cf = Crazyflie()
		# Connect some callbacks from the Crazyflie API
		self.cf.connected.add_callback(self._connected)
		self.cf.disconnected.add_callback(self._disconnected)
		self.cf.connection_failed.add_callback(self._connection_failed)
		self.cf.connection_lost.add_callback(self._connection_lost)

		print "Connecting to %s" % link_uri
		# Try to connect to the Crazyflie
		if self.connected == "False":
			self.cf.open_link(link_uri)
			self.connected = "True"
		# Variable used to keep main loop occupied until disconnect


	def _connected(self, link_uri):
		if self.connected == "True":
			""" This callback is called form the Crazyflie API when a Crazyflie
			has been connected and the TOCs have been downloaded."""
			print "Connected to %s" % link_uri
			self.connected = "False"
			# The definition of the logconfig can be made before connecting
			self.lg_stab = LogConfig(name="Stabilizer", period_in_ms=10)
			self.lg_stab.add_variable("stabilizer.roll", "float")
			self.lg_stab.add_variable("stabilizer.pitch", "float")
			self.lg_stab.add_variable("stabilizer.yaw", "float")
			self.lg_stab.add_variable("stabilizer.thrust", "uint16_t")

			self.cf.log.add_config(self.lg_stab)

			if self.lg_stab.valid:
				self.lg_stab.data_received_cb.add_callback(self.print_stab_data)
				self.lg_stab.start()
			else:
				print 'Could not setup log configuration for stabilizer after connection!'

	def _stab_log_error(self, logconf, msg):
		"""Callback from the log API when an error occurs"""
		print "Error when logging %s: %s" % (logconf.name, msg)

	def _stab_log_data(self, timestamp, data, logconf):
		"""Callback froma the log API when data arrives"""
		print "[%d][%s]: %s" % (timestamp, logconf.name, data)

	def _connection_failed(self, link_uri, msg):
		"""Callback when connection initial connection fails (i.e no Crazyflie
		at the speficied address)"""
		print "Connection to %s failed: %s" % (link_uri, msg)
		self.is_connected = False

	def _connection_lost(self, link_uri, msg):
		"""Callback when disconnected after a connection has been made (i.e
		Crazyflie moves out of range)"""
		print "Connection to %s lost: %s" % (link_uri, msg)

	def _disconnected(self, link_uri):
		"""Callback when the Crazyflie is disconnected (called in all cases)"""
		print "Disconnected from %s" % link_uri
		self.is_connected = False

	def print_stab_data(self, ident, data, logconfig):

		global trim_roll, trim_pitch, trim_yaw

		trim_roll = -(1)*data["stabilizer.roll"]
		trim_pitch = -(1)*data["stabilizer.pitch"]
		trim_yaw = data["stabilizer.yaw"]

		#if trim_roll != 0 or trim_pitch != 0:
		#	trimmed_roll = roll + trim_roll
		#	trimmed_pitch = pitch + trim_pitch
#		else:
#			trimmed_roll = roll
#			trimmed_pitch = pitch

def non_daemon():
	global thrust, pitch, roll, yaw, hold, alive
#	global joy_down,joy_up, joy_right, joy_left, btn_low, btn_high, btn_stop, btn_dummy
	logging.debug('Starting')

	hold = "True"
	thrust = 10000
	i = 0
	crazy = False

	pressed_left = False
	pressed_right = False
	pressed_up = False
	pressed_down = False

	while 1:

#		ser.flushInput()
#		cmd = ser.read(1)

#		joy_down  = ((ord(cmd[0]) & (1 << 0)) >> 0)
#       joy_up    = ((ord(cmd[0]) & (1 << 1)) >> 1)
#       joy_right = ((ord(cmd[0]) & (1 << 2)) >> 2)
#       joy_left  = ((ord(cmd[0]) & (1 << 3)) >> 3)

#       btn_low   = ((ord(cmd[0]) & (1 << 4)) >> 4)
#       btn_high  = ((ord(cmd[0]) & (1 << 5)) >> 5)
#       btn_stop  = ((ord(cmd[0]) & (1 << 6)) >> 6)
#       btn_dummy = ((ord(cmd[0]) & (1 << 7)) >> 7)

#		if btn_stop == 0:
#			thrust = 0
#			alive = "False"
#			hold = "False"
#			print "Exiting main loop in 1 second"
#			time.sleep(0.5)
#			pygame.quit()
#			sys.exit()

#		if btn_high == 0:
#			print "Up"
#			hold = "True"
#			thrust += 2000

#		if btn_low == 0:
#			print "Down"
#			hold = "True"
#			thrust -= 2000
#			if thrust <= 10000:
#				thrust = 10000

#		if joy_up == 0:
#			pitch += 15
#			hold = "False"
#			print "Forward"
#		elif joy_down == 0:
#			pitch -= 15
#			hold = "False"
#			print "Backward"
#		elif joy_left == 0:
#			roll -= 15
#			hold = "False"
#			print "Left"
#		elif joy_down == 0:
#			roll += 15
#			hold = "False"
#			print "Right"
#		else:
#			hold = "True"

#		if btn_dummy == 0:
#			print "Hold"
#			hold = "False"
#			thrust = 40000
#			crazy = not crazy

####################################################################################
		pygame.event.pump()
		for event in pygame.event.get():

			#keys = pygame.key.get_pressed()
			#tab = keys[pygame.K_TAB]
			#ret = keys[pygame. K_RETURN]
			#e = keys[pygame.K_ESCAPE]
			#h = keys[pygame.K_NUMLOCK]
			if event.type == KEYDOWN:          # check for key presses
				if event.key == K_LEFT:        # left arrow turns left
					pressed_left = True
				elif event.key == K_RIGHT:     # right arrow turns right
					pressed_right = True
				elif event.key == K_UP:        # up arrow goes up
					pressed_up = True
				elif event.key == K_DOWN:     # down arrow goes down
					pressed_down = True
				elif event.key == K_ESCAPE:
					thrust = 0
					alive = "False"
					hold = "False"
					print "Exiting main loop in 1 second"
					time.sleep(0.5)
					pygame.quit()
					sys.exit(1)
				elif event.key == K_u:
					print "Up"
					hold = "True"
					thrust += 2000

				elif event.key == K_j:
					print "Down"
					hold = "True"
					thrust -= 2000
					if thrust <= 10000:
						thrust = 10000

				elif event.key == K_h:
					print "Crazy"
					hold = "True"
					#thrust = 40000
					crazy = not crazy


			elif event.type == KEYUP:            # check for key releases
				if event.key == K_LEFT:        # left arrow turns left
					pressed_left = False
				elif event.key == K_RIGHT:     # right arrow turns right
					pressed_right = False
				elif event.key == K_UP:        # up arrow goes up
					pressed_up = False
				elif event.key == K_DOWN:     # down arrow goes down
					pressed_down = False

			if pressed_down == True:
				pitch -= 15
				hold = "False"
				print "Backward"
			elif pressed_up == True:
				pitch += 15
				hold = "False"
				print "Forward"
			elif pressed_left == True:
				roll -= 15
				hold = "False"
				print "Left"
			elif pressed_right == True:
				roll += 15
				hold = "False"
				print "Right"
			else:
				hold = "True"


		#print thrust

		if hold == "True":
			roll = trim_roll 
			pitch = trim_pitch - 2
			

		if crazy == True:
			yaw = i
		else:
			yaw = 0
		
		if i == 359:
			i = 0
		i += 1
		time.sleep(0.01)

	logging.debug('Exiting')

def daemon(test):
	#tesflight = TestFlight("http://kkk.kk");
	#TF = TestFlight(crazyconnected[0][0])
	#global thrust, pitch, roll, yaw, hold
	logging.debug('Starting')
	while alive:

		#setattr(TestFlight,'flightmode.althold', hold)
		#test._cf.param.set_value('flightmode.althold', "True")
		#q.get(thrust, roll, pitch, yaw, hold)

		test.cf.commander.send_setpoint(roll, pitch, yaw, thrust)
		time.sleep(0.01)

	test.cf.close_link()
	logging.debug('Exiting')


def is_number(s):
		try:
			int(s)
			return True
		except ValueError:
			return False


if __name__ == '__main__':

	# connect to serial port
#    ser = serial.Serial()
#	ser.port     = '/dev/ttyUSB0'
#    ser.baudrate = 9600
#    ser.bytesize = serial.EIGHTBITS
#    ser.stopbits = serial.STOPBITS_ONE
#    ser.parity   = serial.PARITY_NONE
#    ser.rtscts   = False
#    ser.xonxoff  = False
#    ser.timeout  = None

#    try:
#        ser.open()
#    except serial.SerialException, msg:
#        print >> sys.stderr, "Could not open serial port:\n" + msg
#        sys.exit(1)

	# Initialize the low-level drivers (don't list the debug drivers)
	cflib.crtp.init_drivers(enable_debug_driver=False)
	# Scan for Crazyflies and use the first one found
	print "Scanning interfaces for Crazyflies..."
	available = cflib.crtp.scan_interfaces()
	print "Crazyflies found:"
	for i in available:
		print i[0]

	if len(available) > 0:

		TF = TestFlight(available[0][0])
		running = "True"
		#q = Queue.Queue()

		d = threading.Thread(name='daemon', target=daemon, args=(TF,))
		d.setDaemon(True)
		t = threading.Thread(name='non-daemon', target=non_daemon)

		time.sleep(5)
		d.start()
		t.start()

		#while 1:
			#thrust = 20000
			#TF.cf.commander.send_setpoint(trimmed_roll, trimmed_pitch, yawrate, thrust)
			#time.sleep(0.01)
			#print thrust


#		while running:
#			if (d.isAlive() == "False") or (t.is_alive() == "False"):
#				running = "False"
#			d.join(1.0)
#			#print 'd.isAlive()', d.isAlive()
#			t.join(1.0)
	else:
		print "No Crazyflies found, cannot run example"
