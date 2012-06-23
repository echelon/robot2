
#include <zmq.hpp>
#include <string>
#include <iostream>
//#include <unistd.h>
#include <stdio.h> // TODO: Remove

#include <cstdlib>
#include "Serial.hpp"

int main(int argc, char** argv)
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

	Serial* robot = new Serial();
	robot->open();

	int counter = 0;

	while(1) {
		std::string msg("");
		switch(counter) {
			case 0:
				msg = "mogo 1:50 2:50\r";
				break;
			case 1:
				msg = "mogo 1:-50 2:50\r";
				break;
			case 3:
				msg = "mogo 1:50 2:-50\r";
				break;
			case 4:
				msg = "mogo 1:-50 2:-50\r";
				break;
			case 5:
				msg = "mogo 1:0 2:0\r";
				break;
			default:
				msg = "mogo 1:0 2:0\r";
				counter = -1;
		}
		counter++;
		printf("Writing to serial... %s\n", msg.c_str());

		robot->write(msg);
		sleep(1);
	}

	return EXIT_SUCCESS;

	while(1) 
	{
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(&request);
        std::cout << "Received Message" << std::endl;
		std::cout << (unsigned char*)request.data() << std::endl;

        //  Do some 'work'
        //sleep(1);

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy((void *)reply.data(), "World", 5);
        socket.send(reply);
    }

	return EXIT_SUCCESS;
}

