
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
//#include <unistd.h>
#include <stdio.h> // TODO: Remove

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

int main(int argc, char** argv)
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

	Serial* robot = 0;

	robot = openSerial();

	if(!robot) {
		std::cerr << "Couldn't open USB Serial. Shutting down." << std::endl;
		return EXIT_FAILURE;
	}

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

		d = (char*)request.data();

		if(!strcmp(d, "w")) {
			msg = "mogo 1:50 2:50\r";
		}
		else if(!strcmp(d, "s")) {
			msg = "mogo 1:-50 2:-50\r";
		}
		else if(!strcmp(d, "a")) {
			msg = "mogo 1:50 2:-50\r"; // TODO: Not sure of direction
		}
		else if(!strcmp(d, "d")) {
			msg = "mogo 1:-50 2:50\r";
		}
		else if(!strcmp(d, "e")) {
			msg = "mogo 1:0 2:0\r";
		}

		if(msg.length()) {
			printf("Writing %s\n", msg.c_str());
			robot->write(msg);
		}


        //  Do some 'work'
        //sleep(1);

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy((void *)reply.data(), "World", 5);
        socket.send(reply);
    }

	return EXIT_SUCCESS;
}

