#!/usr/bin/env python
"""
Relay Joystick events as instructions to the robot.

Questions: 
	* Should the script ramp up speed the longer something is pressed, 
	  or should the daemon do that work? 

	* Factor in the degree to which the joystick is pressed? 

	* Should the script send a 'stop' command, or should the daemon do
	  that work as well? (Daemon assumes that when no input is received
	  after X amount of time, it should shut down the motors.)

A work in progress.
"""

# XXX: **Important Note about PyGame:
#	Printing is done in a different thread than SDL, which may
#	cause printing to appear slow. In reality, it isn't. 

import sys
import pygame
from zmq_connect import *

def joystick_handle(e):
	if e.type == pygame.JOYAXISMOTION:
		print "Motion"
		pass
	print "TODO"

def main():
	"""
	Main
	Use joystick to control robot. 
	"""
	socket = connect_robot_daemon()

	pygame.joystick.init()
	pygame.display.init()

	count = pygame.joystick.get_count()
	if not count:
		print "No Joystick!"
		sys.exit()

	print count

	js = pygame.joystick.Joystick(0)
	js.init()
	print js.get_name()

	numButtons = js.get_numbuttons()
	print "NumButtons: %d" % numButtons

	while True:
		e = pygame.event.wait()
		if e.type in [pygame.JOYBUTTONDOWN]:

			for i in range(numButtons):
				if not js.get_button(i):
					continue

				# Up, Right, Down, Left (respectively)
				if i in [4, 5, 6, 7]:
					if i == 4:
						socket.send("w")
					elif i == 5:
						socket.send("d")
					elif i == 6:
						socket.send("s")
					elif i == 7:
						socket.send("a")

					message = socket.recv()
					continue

				# Triangle, Circle, X, Square (respectively)
				elif i in [12, 13, 14, 15]:
					socket.send("e") # Stop motors
					message = socket.recv()
					continue

			#joystick_handle(e)
			#socket.send('e')

		#if ch in ['q', 'Q']:
		#	sys.exit()

		
		#  Get the reply.
		#message = socket.recv()
		#print "Received reply [", message, "]"

if __name__ == '__main__': main()
