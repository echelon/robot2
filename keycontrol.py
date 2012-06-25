#!/usr/bin/env python
"""
Grab keyboard presses and relay them as instructions to the robot. 

Key combinations for movement:
	* wasd (classic)
	* hjkl (vim-like)
	* up, down, left, right

Questions: 
	* Should the script ramp up speed the longer something is pressed, 
	  or should the daemon do that work? 

	* Should the script send a 'stop' command, or should the daemon do
	  that work as well? (Daemon assumes that when no input is received
	  after X amount of time, it should shut down the motors.)

A work in progress.
"""

import zmq
import sys
#import select # XXX: Investigate 'select' library. 

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
	context = zmq.Context()

	# Socket to talk to server
	print 'Connecting to server.'
	socket = context.socket(zmq.REQ)
	socket.connect('tcp://localhost:5555')

	print 'Input keypresses; \'q\' to exit.'

	while True:
		ch = getch()
		if ch in ['q', 'Q']:
			sys.exit()

		socket.send(ch)
		
		#  Get the reply.
		message = socket.recv()
		print "Received reply [", message, "]"

if __name__ == '__main__': main()
