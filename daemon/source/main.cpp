/**
 * Copyright (c) 2012 Brandon Thomas
 * http://possibilistic.org | echelon@gmail.com
 * See README.md for license information. 
 *
 * Description
 * 
 * 		main
 * This is the main program that controls initialization and spawns the 
 * threads that control the robot over serial and listen for ZeroMQ messages.
 *
 * Linux-specific.  
 * 
 * TODO: This class is a mess. Clean it up. 
 */

#include <zmq.hpp>
#include <pthread.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stdio.h> // TODO: Remove
#include <time.h> // Get system time
#include <cstdlib>
#include <cstring> // TODO: Ugh, remove
#include "Serial.hpp"
#include "Serializer.hpp"
#include "picojson.h"

/**
 * Constants
 */
const int MSECONDS_TIMEOUT = 200;

// Non-differential movement
// TODO: Not sure of left and right directions. 
const std::string ROBOT_FORWARD  = "mogo 1:50 2:50\r";
const std::string ROBOT_BACKWARD = "mogo 1:-50 2:-50\r";
const std::string ROBOT_LEFT 	 = "mogo 1:50 2:-50\r";
const std::string ROBOT_RIGHT	 = "mogo 1:-50 2:50\r";
const std::string ROBOT_STOP	 = "mogo 1:0 2:0\r";


/**
 * Nasty globals
 */
Serializer* robot = 0;

// These are for communicating between threads.
pthread_mutex_t messageMut = PTHREAD_MUTEX_INITIALIZER;
std::string messageStr = "";
timespec messageTime;

/**
 * See if a number of milliseconds has elapsed since the timestamp.
 */
bool msec_elapsed(timespec then, int ms)
{
	timespec now;
	long now_ms;
	long then_ms;

	clock_gettime(CLOCK_REALTIME, &now);

	// Create millisecond-resolution timestamps
	now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
	then_ms = then.tv_sec * 1000 + then.tv_nsec / 1000000;

	return (now_ms - then_ms > ms);
}

/**
 * Send a robot a _simple_ motor command.
 * Inputs are 'wasd' and 'e'. 
 * TODO: I have to do more sophisticated than this; 
 *       such as speed differentials
 * FIXME: relies on global
 */
void robot_send_command(const char* msg)
{
	// FIXME: Very basic, unsophisticated
	if(!strcmp(msg, "w")) {
		robot->motor(100, 100);
	}
	else if(!strcmp(msg, "s")) {
		robot->motor(-100, -100);
	}
	else if(!strcmp(msg, "a")) {
		robot->motor(100, 0);
	}
	else if(!strcmp(msg, "d")) {
		robot->motor(0, 100);
	}
	else if(!strcmp(msg, "e")) {
		robot->stop();
	}
}

/**
 * Thread that responds to ZMQ messages and relays them 
 * to the robot.
 */
void* ZmqServerThread(void* n)
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

	// TODO: This is going to have to be a whole lot more robust. ie, 
	// 		 auto shutdown of the motors, heuristics, direct control vs 
	// 		 custom protocol (in JSON), etc. 
	// TODO: Should the serial motor control be a separate thread? 
	//			I think so...
	while(1) 
	{
        zmq::message_t request;
		char* d = 0;
		std::string msg("");
		std::string commandType("");

        //  Wait for next request from client
        socket.recv(&request);

		const char* base = static_cast<const char*>(request.data());
        std::string json(base, base + request.size());

		std::cout << json << std::endl;

		// TODO TODO TODO TODO -- TYPE CHECKING FOR JSON MUST BE VERY THOROUGH

		picojson::value v, v2, v3, v4;

		std::string err;
		picojson::parse(v, base, base + strlen(base), &err);
		if (! err.empty()) {
			std::cerr << err << std::endl;
		}

		if(v.is<picojson::object>()) {
			std::cout << "It's an object" << std::endl;
		}

		// XXX ????? What to do ???

		const picojson::value::object& obj = v.get<picojson::object>();

		for(picojson::value::object::const_iterator i = obj.begin();
			 i != obj.end(); i++) {
		  std::cout << i->first << ": " << i->second.to_str() << std::endl;
		}

		std::cout << obj.find("cmd")->first << std::endl;

		v2 = obj.find("cmd")->second;

		// XXX: GET MOTOR COMMAND ! 
		if(v2.is<std::string>()) {
			std::cout << "It's a string" << std::endl;
		}

		std::string cmdStr;
		cmdStr = v2.get<std::string>();

		std::cout << cmdStr << std::endl;

		picojson::array arrayV;

		double doubleV;

		if(obj.count("params")) {
			v2 = obj.find("params")->second; // TODO: Rename v2 to picoval

			std::cout << "Has params" << std::endl;
			if(v2.is<picojson::array>()) {

				std::cout << "Has array" << std::endl;
				arrayV = v2.get<picojson::array>();

				for(int i = 0; i < arrayV.size(); i++) {

					if(arrayV[i].is<double>()) {
						doubleV = arrayV[i].get<double>();
						std::cout << doubleV << std::endl;
					}
				}
			}
		}



		//v2 = v.object["cmd"];
		//std::cout << v2.get<std::string>() << std::endl;
		//std::cout << v.object["cmd"].get<std::string>() << std::endl;





		d = (char*)request.data();

		pthread_mutex_lock(&messageMut);
		messageStr = d;
		clock_gettime(CLOCK_REALTIME, &messageTime);
		pthread_mutex_unlock(&messageMut);

        // Send reply back to client
		// TODO: Fix so not required.
        zmq::message_t reply(5);
        memcpy((void *)reply.data(), "messg", 5);
        socket.send(reply);
    }

	pthread_exit(0);
}

/**
 * Thread that responds to certain timeouts and relays
 * them to the robot.
 * TODO/FIXME: RENAME
 */
void* TimeThread(void* n)
{
	timespec lastWritten_command;
	timespec lastWritten_timeout;

	enum LastCommand { COMMAND_FROM_NETWORK, COMMAND_FROM_TIMEOUT };
	LastCommand lastCommandType = COMMAND_FROM_TIMEOUT;
	std::string lastCommand("");

	while(1)
	{
		timespec t;
		std::string m;

		// Any commands from the network? 
		pthread_mutex_lock(&messageMut);
		t = messageTime;
		m = messageStr;
		pthread_mutex_unlock(&messageMut);

		// Timeouts.
		if(msec_elapsed(t, MSECONDS_TIMEOUT)) {
			// Don't flood serial unless timeout is a new event
			if(lastCommandType != COMMAND_FROM_TIMEOUT || 
					msec_elapsed(lastWritten_timeout, 100)) {
				robot_send_command("e");
				clock_gettime(CLOCK_REALTIME, &lastWritten_timeout);
				lastCommandType = COMMAND_FROM_TIMEOUT;
			}
			continue;
		}

		// Commands.
		if(m.length()) {
			// Don't flood serial
			if(msec_elapsed(lastWritten_command, 100)) {
				robot_send_command(m.c_str());
				clock_gettime(CLOCK_REALTIME, &lastWritten_command);
			}
			// Ensure new commands get written
			else if(lastCommand.compare(m)) {
				robot_send_command(m.c_str());
				clock_gettime(CLOCK_REALTIME, &lastWritten_command);
			}
			lastCommand = m;
			lastCommandType = COMMAND_FROM_NETWORK;
		}
	}

	pthread_exit(0);
}

/**
 * Try to open one of the first ten serial ports.
 * It's probably the first serial device (0), but just in case...
 */
Serializer* openSerial()
{
	Serial* s = 0;
	Serializer* r = 0;
	
	for(int i = 0; i < 10; i++) {
		s = new Serial(i);
		try {
			std::cout << "Opening serial on #" << i << std::endl;
			s->open();
		}
		catch(const std::runtime_error& e) {
			std::cerr << "Could not open serial on #" << i << std::endl;
			delete s;
			s = 0;
			continue;
		}
		break;
	}

	if(s) {
		r = new Serializer(s); // FIXME: This will memleak, but it's okay for now
	}

	return r;
}

/**
 * Main Routine
 */
int main(int argc, char** argv)
{

	pthread_t zmqThread;
	pthread_t timeThread;

	robot = openSerial();

	if(!robot) {
		std::cerr << "Couldn't open USB Serial. Shutting down." << std::endl;
		return EXIT_FAILURE;
	}

	pthread_create(&zmqThread, 0, ZmqServerThread, 0);
	pthread_create(&timeThread, 0, TimeThread, 0);

	pthread_join(zmqThread, 0);
	pthread_join(timeThread, 0);

	return EXIT_SUCCESS;
}

