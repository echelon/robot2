#!/usr/bin/env python

"""
Relay Joystick events as instructions to the robot.
A work in progress. Need to support more than just PS3 controller.

Questions: 
	* Should the script ramp up speed the longer something is pressed, 
	  or should the daemon do that work? 

	* Factor in the degree to which the joystick is pressed? 
"""

# XXX: **Important Note about PyGame**
#	Printing is done in a different thread than SDL, which may cause printing
#	to the console to appear slow. In reality, it isn't. Keep this in mind
#	when debugging the program. 

# TODO: Support other joysticks and gamepads. 

# FIXME: There is some latency with the joypad that causes motor stuttering. 

import sys
import pygame
from zmq_connect import *

def main():
	"""
	Main
	Use joystick to control robot. 
	"""
	socket = connect_robot_daemon()

	pygame.joystick.init()
	pygame.display.init()

	if not pygame.joystick.get_count():
		print "No Joystick detected!"
		sys.exit()

	js = pygame.joystick.Joystick(0)
	js.init()

	print js.get_name()

	numButtons = js.get_numbuttons()

	debugFile = open("DebugFile.txt", "w+")

	"""
	Events are being sent to Pygame as long as a button is pressed
	or a joystick is held. KEYUP and KEYDOWN only fire once, but there
	are still event types sent.
	"""
	while True:
		e = pygame.event.wait()

		# Not sure why Axis 8 continues to be reported.
		if e.type == pygame.JOYAXISMOTION and e.axis == 8:
			continue

		debugFile.write(str(e)+"\n")
		debugFile.flush()

		if e.type == pygame.JOYAXISMOTION:
			print e.value
			print e.axis
			#debugFile.write(str(e)+"\n")
			#debugFile.flush()

		# Joypad Buttons. 
		for i in range(numButtons):
			if not js.get_button(i):
				continue

			msg = None

			# Up, Right, Down, Left (respectively)
			if i in [4, 5, 6, 7]:
				if i == 4:
					msg = "w"
				elif i == 5:
					msg = "d"
				elif i == 6:
					msg = "s"
				elif i == 7:
					msg = "a"

			# Triangle, Circle, X, Square (respectively)
			# Use these to stop the robot motors
			elif i in [12, 13, 14, 15]:
				msg = "e"

			# Use PS button to shut down program
			elif i == 16:
				print "Sending stop and exiting script"
				socket.send("e")
				socket.recv()
				sys.exit()

			if msg:
				socket.send(msg)
				socket.recv() # FIXME: Fix comms model so I won't need

if __name__ == '__main__': main()
