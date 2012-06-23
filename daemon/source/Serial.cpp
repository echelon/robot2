/**
 * Copyright (c) 2008 - 2010 Brandon Thomas
 * http://possibilistic.org | echelon@gmail.com
 * See README.md for license information. 
 * TODO: This class is a mess. Clean it up. 
 */

#include "Serial.hpp"

#include <fcntl.h> // open(), etc.
#include <sys/select.h> // select()
#include <stdexcept>
#include <sstream>
#include <stdio.h>
#include <time.h>

Serial::Serial(int num):
	fname(""),
	fd(0),
	options(),
	lastWrite(),
	mutex((pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
{
	std::ostringstream os;

	os << "/dev/ttyUSB" << num;
	fname = os.str();

	//open(); TODO: Commented out. User should open connection.
}

void Serial::open()
{
	if(isOpen()) {
		// TODO use C++ features
		fprintf(stderr, "Serial is already open\n");
		return;
	}

	// TODO: Document why
	fd = ::open(fname.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK); 

	if (fd == -1) {
		// TODO use fname.
		throw std::runtime_error("Unable to open /dev/ttyUSB#.");
	}

	// Set serial connection options for Serializer.
	// "The SerializerTM ships communicating at 19200 baud, 8 data bits, 
	//  1 stop bit, No Parity, and no Flow Control."

	// alter the current options
    tcgetattr(fd, &options);

	// 19200 baud
    cfsetispeed(&options, B19200);
    cfsetospeed(&options, B19200);

	// Control flags
	options.c_cflag |= CLOCAL;		// Ignore status lines
	options.c_cflag |= CREAD;		// Enable receiver
	options.c_cflag |= CS8;			// Eight data bits
	options.c_cflag &= ~CSTOPB;		// One stop bit (vs two)
	options.c_cflag &= ~(PARENB|PARODD); // No parity
	options.c_cflag &= ~CRTSCTS;	// No RTS/CTS control flow

	// Input flags
	options.c_iflag &= ~(INLCR|IGNCR|ICRNL|IGNBRK);
	options.c_iflag &= ~(INPCK|ISTRIP);
	options.c_iflag &= ~(IXON|IXOFF|IXANY);	// No software flow control
    options.c_iflag &= ~IUCLC;
    options.c_iflag &= ~PARMRK;

	// Output and local mode flags
	options.c_oflag &= ~(OPOST);
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ISIG|IEXTEN);

	// flush I/O & set options
	tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &options);
}

void Serial::flush(int queue)
{
	// Flush input, output, or both
	if(queue != TCIFLUSH && queue != TCOFLUSH && queue != TCIOFLUSH) {
		queue = TCIOFLUSH;
	}
	tcflush(fd, queue);
}

std::string Serial::read(unsigned int bytes)
{
	std::string ret = "";

	if(!isOpen()) {
		// TODO: Use C++ features
		fprintf(stderr, "Serial, Can't read from a non-open file.\n");
		return "";
	}

	pthread_mutex_lock(&mutex);
	ret = doRead(bytes);
	flush(); // TODO (Still not working?)
	pthread_mutex_unlock(&mutex);

	return ret;
}

bool Serial::write(std::string data, bool priority)
{
	bool ret;

	if(!isOpen()) {
		fprintf(stderr, "Serial, Can't write to a non-open file.\n");
		return false;
	}

	pthread_mutex_lock(&mutex);
	ret = doWrite(data, priority);
	flush(); // TODO (Still not working?)
	pthread_mutex_unlock(&mutex);

	return ret;
}

std::string Serial::writeRead(std::string inBuff, unsigned int readBytes)
{
	bool wrRet;
	std::string rdRet = "";

	pthread_mutex_lock(&mutex);
	wrRet = doWrite(inBuff);

	if(!wrRet) {
		pthread_mutex_unlock(&mutex);
		fprintf(stderr, "Serial::writeRead, DID NOT WRITE\n"); // keep
		return "";
	}

	rdRet = doRead(readBytes);
	flush(); // TODO (Still not working?)

	pthread_mutex_unlock(&mutex);
	
	return rdRet;
}

int Serial::select(int microseconds, int seconds, bool chkRead, 
		bool chkWrite, bool chkError)
{
	struct timeval tv;
	fd_set rfds; // Check if line is available to read
	fd_set wfds; // Check if line is available to write
	fd_set efds; // Check if line has an error condition pending

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (chkRead) {
		FD_SET(fd, &rfds);
	}
	if(chkWrite) {
		FD_SET(fd, &wfds);
	}
	if(chkError) {
		FD_SET(fd, &efds);
	}

	tv.tv_sec = seconds;
	tv.tv_usec = microseconds;

	int r = ::select(fd+1, &rfds, &wfds, &efds, &tv);

	return r;
}

// TODO XXX FIXME TODO: THIS IS REALLY UGLY! GAH!
std::string Serial::doRead(unsigned int bytes)
{
	std::string buff;
	int failcnt = 0;

	if(bytes < 1) {
		return 0;
	}

	while(buff.length() < bytes) 
	{
		int r = select(50500, 0, true, false, false);
		if(r < 0) {
			printf("Serial::doRead, Error with select()\n");
			return 0; // TODO EXCEPTION
		}
		else if(r == 0) {
			printf("Serial::doRead, LINE NOT AVAILABLE!!!!\n");
			break;
		}

		char rbuf[1000];
		int re = ::read(fd, rbuf, bytes-buff.length());
		if(!re) {
			if(failcnt >= 5) {
				printf("Serial::read, Fail count over 5.\n");
				return 0; // DON'T RETURN PARTIAL DATA :(
			}
			failcnt++;
			continue;
		}
		failcnt = 0;

		buff += rbuf;

		// TODO NOTE: PySerial had an additional check here for timeouts.
		// I think my code is fine, but if errors occur this may be a reason.
	}
	//tcflush(fd, TCIOFLUSH);

	return buff;
}

bool Serial::doWrite(std::string data, bool priority)
{
	//
	// TODO/NOTE: Timing was removed. 
	// If needed, can see old code in diffs. (4/21/09)
	//

	std::string buff(data);
	int len = buff.length();

	while(len > 0) 
	{
		int r = select(60500, 0, false, true, false);
		if(r < 0) {
			printf("Serial::doWrite, Error with select()\n");
			return false; // TODO EXCEPTION
		}
		else if(r == 0) {
			printf("Serial::doWrite, LINE NOT AVAILABLE!!!!!\n");
			return false;
		}

		int written = ::write(fd, buff.c_str(), len);
		buff.erase(0, written);
		len -= written;
	}
	clock_gettime(CLOCK_REALTIME, &lastWrite);
	return true;
}

