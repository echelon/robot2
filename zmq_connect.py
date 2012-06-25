"""
Module for ZeroMQ-related routines, etc.
"""

import zmq

# TODO: Error handling
# TODO: More sophisticated ZeroMQ usage

def connect_robot_daemon(host = 'tcp://localhost:5555'):
	"""
	Connect to the robot deamon and return the socket. 
	"""
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect(host)
	return socket

