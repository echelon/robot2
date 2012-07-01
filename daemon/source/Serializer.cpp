#include "Serializer.hpp"
#include "Command.hpp"
#include "Serial.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

std::string Serializer::getInstruction(Command cmd)
{
	std::string instr;
	std::ostringstream os;
	std::vector<int> params = cmd.getParams();

	switch(cmd.getType()) {
		case COMMAND_NONE:
			break;
		case COMMAND_MOTOR:
			os << "mogo 1:" << params[0] << " 2:" << params[1] << "\r";
			//os << "pwm 1:" << params[0] << " 2:" << params[1] << "\r";
			instr = os.str();
			break;
		case COMMAND_BLINK:
			os << "blink 1:" << params[0] << " 2:" << params[1] << "\r";
			instr = os.str();
			break;
		case COMMAND_BLINK_ONE:
			os << "blink " << params[0] << ": " << params[1] << "\r";
			instr = os.str();
			break;
		case COMMAND_STOP:
			instr = "stop\r";
			break;

		case COMMAND_SIMPLE_W:
		case COMMAND_SIMPLE_A:
		case COMMAND_SIMPLE_S:
		case COMMAND_SIMPLE_D:
		case COMMAND_SIMPLE_E:
			break; // TODO
	}

	std::cout << instr << std::endl;

	return instr;
}

void Serializer::blink(unsigned int rate1, unsigned int rate2) 
{
	std::ostringstream os;

	// Invalid commands
	if(rate1 > 127 || rate2 > 127) {
		return;
	}
	
	os << "blink 1:" << rate1 << " 2:" << rate2 << "\r";
	serial->write(os.str());
}

void Serializer::blinkOne(unsigned int ledId, unsigned int rate)
{
	std::ostringstream os;

	// Invalid commands
	if((ledId != 1 && ledId != 2) || rate > 127) {
		return;
	}
	
	os << "blink " << ledId << ":" << rate << "\r";
	serial->write(os.str());
}

void Serializer::motor(int vel1, int vel2)
{
	std::ostringstream os;

	os << "mogo 1:" << vel1 << " 2:" << vel2 << "\r";
	serial->write(os.str());
}

void Serializer::stop()
{
	serial->write("stop\r");
}
