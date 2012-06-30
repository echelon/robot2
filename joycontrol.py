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

def debug_print(s):
	"""
	Print output to a debug file (Call tail -f to monitor it)

	Since the local PyGame was compiled with SDL debugging, the terminal gets
	filled with junk and becomes useless . This is kind of necessary to debug.
	"""
	global _debugFile

	def openFile():
		return open("DebugFile.txt", "w+")

	try:
		if not _debugFile:
			_debugFile = openFile()
	except:
		_debugFile = openFile()

	_debugFile.write(str(s)+"\n")
	_debugFile.flush()

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

	#debugFile = open("DebugFile.txt", "w+")

	"""
	Events are being sent to Pygame as long as a button is pressed
	or a joystick is held. KEYUP and KEYDOWN only fire once, but there
	are still event types sent.

			--- WRONG! This was pressure sensitivity being sent as Axes. 
	"""

	"""
	PS3 Mappings
	------------
	Axis 0	- Left horiontal (-1 is up, 1 is down)
	Axis 1	- Left vertical
	Axis 2	- Right horizontal 
	Axis 3	- Right vertical 

	Button 4	- Up
	Button 5	- Right
	Button 6	- Down
	Button 7	- Left

	Button 12	- Triangle
	Button 13	- Circle
	Button 14	- X
	Button 15	- Square 

	Button 16	- PS Button
	"""
	while True:
		e = pygame.event.get()

		"""
		# Do not count button sensitivity as axes!
		if e.type == pygame.JOYAXISMOTION and e.axis in [8, 9, 10, 11]:
			continue

		debugFile.write(str(e)+"\n")
		debugFile.flush()

		if e.type == pygame.JOYAXISMOTION:
			print e.value
			print e.axis
			#debugFile.write(str(e)+"\n")
			#debugFile.flush()
		"""

		# Only buttons OR the joypad may be used.
		# We try the joypad first. 
		moved = False

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
				continue # Ignore joysticks 

		"""
		If joysticks are being used, the control is similar to that of the 
		videogame 'Katamari', wherein the vertical direction of each joystick
		controls the velocity of each motor. The horizontal component of the
		joysticks is ignored. 
		"""
		leftv = js.get_axis(1)
		rightv = js.get_axis(3)

		#debug_print("Joysticks: %f and %f" % (leftv, rightv))

		# TODO/FIXME: This is very crude. Ultimately, we're going to do
		# far better than this pathetic method of joystick control. 
		msg = None
		if leftv or rightv:
			# Forward or reverse
			if leftv and rightv:
				if leftv < 0 and rightv < 0:
					msg = "w"
				elif leftv > 0 and rightv > 0:
					msg = "s"
			elif leftv:
				msg = "a"
			elif rightv:
				msg = "d"

		if msg:
			socket.send(msg)
			socket.recv() # FIXME: Fix comms model so I won't need

if __name__ == '__main__': main()
