
#include <zmq.hpp>
#include <string>
#include <iostream>
//#include <unistd.h>

#include <cstdlib>

int main(int argc, char** argv)
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

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

