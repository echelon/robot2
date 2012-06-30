#!/usr/bin/env python
"""
Grab keyboard presses and relay them as instructions to the robot. 

Key combinations for movement:
	* wasd (classic)
	* hjkl (vim-like)				-- TODO
	* up, down, left, right			-- TODO
	* e (stop motors)
	* q (stop motors and exit script)

Questions: 
	* Should the script ramp up speed the longer something is pressed, 
	  or should the daemon do that work? 

	* Should the script send a 'stop' command, or should the daemon do
	  that work as well? (Daemon assumes that when no input is received
	  after X amount of time, it should shut down the motors.)

A work in progress.
"""

import sys
import pygame
import json
from zmq_connect import *

# XXX: The 'pause' when switching keytypes is due to the keyboard delay
# Whenever a new key is pressed, the terminal causes a short pause. 

def getch():
	"""
	Grab a character from the console. (Unix-only)
	Taken from http://stackoverflow.com/a/1394994
	"""
	import sys, tty, termios
	fd = sys.stdin.fileno()
	old = termios.tcgetattr(fd)
	try:
		tty.setraw(fd)
		return sys.stdin.read(1)
	finally:
		termios.tcsetattr(fd, termios.TCSADRAIN, old)

def main():
	"""
	Main
	On keypress, sends robot commands to daemon.
	"""
	socket = connect_robot_daemon()

	print 'Input keypresses; \'q\' to exit.'

	while True:
		ch = getch()
		if ch in ['q', 'Q']:
			socket.send('e')
			socket.recv()
			sys.exit()

		print "Input: %s" % ch

		socket.send(ch)
		socket.recv()

socket = connect_robot_daemon() # TODO FIXME MOVE

def send_robot_json(cmd, params=None, raw=None):
	"""
	Command (cmd) is one of the following strings:
		motor, stop, blink, blinkLed, raw

	Parameters encoded as list/tuple for the following commands:
		* motor: [velocity1, velocity2]
		* blink: [rate1, rate2] - rate is between 0 and 127
		* blinkLed: [id, rate] - id is 1 or 2, rate is as above
	"""

	if cmd not in ('motor', 'stop', 'blink', 'blinkLed', 'raw'):
		print "Invalid command for JSON protocol."
		return 

	obj = {'cmd': cmd}

	if params and type(params) in [tuple, list]:
		obj['params'] = params

	elif raw and command == 'raw':
		obj['raw'] += raw

	jsobj = json.dumps(obj, indent=4) # TODO: Remove indent to make faster
	#print jsobj

	socket.send(jsobj)
	socket.recv()


def main_pygame():
	"""
	Main (using Pygame)
	On key event, send robot commands to daemon.
	"""


	def send_robot(ch):
		socket.send(ch)
		socket.recv()

	print 'Input keypresses; \'q\' to exit.'

	pygame.init()
	#pygame.display.init()
	screen = pygame.display.set_mode((640, 480))
	pygame.display.set_caption('Untitled')

	while True:
		pygame.event.pump()
		e = pygame.key.get_pressed()

		if e[pygame.K_w]:
			#send_robot('w')
			send_robot_json('motor', [100, 100])
		elif e[pygame.K_a]:
			#send_robot('a')
			send_robot_json('motor', [100, 0])
		elif e[pygame.K_s]:
			#send_robot('s')
			send_robot_json('motor', [-100, -100])
		elif e[pygame.K_d]:
			#send_robot('d')
			send_robot_json('motor', [0, 100])
		elif e[pygame.K_e]:
			#send_robot('e')
			send_robot_json('stop')
		elif e[pygame.K_q]:
			print "Exiting..."
			#send_robot('e')
			send_robot_json('stop')
			sys.exit()

if __name__ == '__main__': main_pygame()
