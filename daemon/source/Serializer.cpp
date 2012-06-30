#include "Serializer.hpp"
#include "Serial.hpp"

#include <string>
#include <sstream>

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
