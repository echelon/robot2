
#include <zmq.hpp>
#include <pthread.h>
#include <string>
#include <iostream>
#include <stdexcept>
//#include <unistd.h>
#include <stdio.h> // TODO: Remove
#include <time.h> // Get system time, sleep()
#include <cstdlib>
#include <cstring> // TODO: Ugh, remove
#include "Serial.hpp"

/**
 * Try to open one of the first ten serial ports.
 */
Serial* openSerial()
{
	Serial* s = 0;
	
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
	return s;
}

// FIXME: No globals! 
Serial* robot = 0;
time_t lastCommand; // XXX: Not working
timespec shutdownTime; // XXX: BETTER?
pthread_mutex_t lastCommandMutex = PTHREAD_MUTEX_INITIALIZER;

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

	std::string lastCommandType = ""; // FIXME: Meh
	timespec lastCommandTime;

	// TODO: This is going to have to be a whole lot more robust. ie, 
	// 		 auto shutdown of the motors, heuristics, direct control vs 
	// 		 custom protocol (in JSON), etc. 
	// TODO: Should the serial motor control be a separate thread? 
	while(1) 
	{
        zmq::message_t request;
		char* d = 0;
		std::string msg("");

        //  Wait for next request from client
        socket.recv(&request);
        std::cout << "Received Message" << std::endl;
		std::cout << (char*)request.data() << std::endl;

		// TODO: REORGANIZE
		// TODO: Should go into class, wrapped in get() and set()
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		pthread_mutex_lock(&lastCommandMutex);
		//lastCommand = now.tv_sec; 

		// Try this instead. Give 100ms of leeway. 
		//shutdownTime = now;
		clock_gettime(CLOCK_REALTIME, &shutdownTime);
		//std::cout << "Sec: " << shutdownTime.tv_sec << std::endl;
		//std::cout << "Nsec: " << shutdownTime.tv_nsec << std::endl;
		shutdownTime.tv_nsec += 1000 * 2000;
		shutdownTime.tv_sec += 5;
		pthread_mutex_unlock(&lastCommandMutex);


		d = (char*)request.data();

		if(!strcmp(d, "w")) {
			msg = "mogo 1:50 2:50\r";
			lastCommandType = "w";
		}
		else if(!strcmp(d, "s")) {
			msg = "mogo 1:-50 2:-50\r";
			lastCommandType = "s";
		}
		else if(!strcmp(d, "a")) {
			msg = "mogo 1:50 2:-50\r"; // TODO: Not sure of direction
			lastCommandType = "a";
		}
		else if(!strcmp(d, "d")) {
			msg = "mogo 1:-50 2:50\r";
			lastCommandType = "d";
		}
		else if(!strcmp(d, "e")) {
			msg = "mogo 1:0 2:0\r";
			lastCommandType = "e";
		}

		if(msg.length()) {
			printf("Writing %s\n", msg.c_str());

			// Make sure we don't flood the robot with a constant stream
			if(now.tv_sec - lastCommandTime.tv_sec > 0) {
				robot->write(msg);
				lastCommandTime = now;
			}
		}

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy((void *)reply.data(), "World", 5);
        socket.send(reply);
    }

	pthread_exit(0);
}

/**
 * Thread that responds to certain timeouts and relays
 * them to the robot.
 */
void* TimeThread(void* n)
{
	timespec now;
	timespec shutdown;

	while(1) {
		clock_gettime(CLOCK_REALTIME, &now);

		//std::cout << "Seconds: " << now.tv_sec << std::endl;

		// TODO - move into own class
		pthread_mutex_lock(&lastCommandMutex);
		shutdown = shutdownTime;
		pthread_mutex_unlock(&lastCommandMutex);

		std::cout << "Now:\t\t" << now.tv_nsec << std::endl;
		std::cout << "Shutdown:\t" << shutdown.tv_nsec << std::endl;
		std::cout << std::endl;

		// Shutdown motors for not being actively controlled
		//if(now.tv_sec - then >= 1) {
		if(now.tv_sec >= shutdownTime.tv_sec && 
		   now.tv_nsec > shutdownTime.tv_nsec) {
				std::cout << "SHUT DOWN MOTORS" << std::endl;
				robot->write("mogo 1:0 2:0\r");

				// DO NOT FLOOD SERIAL WITH SHUTDOWN COMMANDS!
				// Here's an easy hack not to ...
				sleep(1); 
		}
		else {
		}

	}

	pthread_exit(0);
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

	zmqThread  = pthread_create(&zmqThread, 0, ZmqServerThread, 0);
	timeThread = pthread_create(&timeThread, 0, TimeThread, 0);

	while(1) {
		//wait(1);
	}

	return EXIT_SUCCESS;
}

