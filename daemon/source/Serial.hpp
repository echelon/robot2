#ifndef ROBOT2_SERIAL
#define ROBOT2_SERIAL
/**
 * Copyright (c) 2008 - 2012 Brandon Thomas
 * http://possibilistic.org | echelon@gmail.com
 * See README.md for license information. 
 *
 * Description
 * 
 * 		Serial
 * A mutex-locked access object for USB serial I/O devices. Uses termio and 
 * pthread mutexes. (The preset baud rates/parity bits may only work for the 
 * RCSerializer, however.) 
 *
 * Linux-specific.  
 * 
 * TODO: This class is a mess. Clean it up. 
 *
 * TODO: Make this a monolithic class for RCSerializer. Forget OO junk until
 * 		 we need Arduino control. 
 */

#include <termios.h>
#include <pthread.h>
#include <string>

class Serial
{
	public:
		/**
		 * CTOR
		 * Opens /dev/ttyUSB{num}
		 */
		Serial(int num = 0);

		/**
		 * TODO CTOR
		 * Opens device specified by string.
		 */
		Serial(std::string filename);

		/**
		 * DTOR
		 * Closes connection. 
		 * TODO: Send shutdown commands. 
		 */
		~Serial() { close(); };

		/**
		 * Open the connection
		 * Sets up correct baud rates, parity, etc. for USB Serial
		 */
		void open();

		// If the device is open
		bool isOpen() { return (fd > 0); };

		// Close the device.
		void close() { if(isOpen()) { ::close(fd); } };

		/**
		 * Flush a queue with tcflush. Provide the queue to be flushed,
		 * or both input and output queues are flushed by default.
		 * TODO: Better documentation
		 */
		void flush(int queue = TCIOFLUSH);

		/**
		 * Read a specified number of bytes from the line.
		 */
		std::string read(unsigned int bytes = 1000);

		/**
		 * TODO: New DOC --
		 * Write a string to the line. 
		 * XXX: OLD DOC
		 * Write a specified number of bytes of a character buffer 
		 * to the line.
		 * @priority messages skip ahead in queues (if any) and don't 
		 * get rejected
		 * @return bool wasWritten
		 */
		bool write(std::string, bool priority = false); 

		/**
		 * Write to the line, then read the response.
		 */
		std::string writeRead(std::string in, unsigned int rbytes = 1000);

	protected:
		// Filename of the device.
		std::string fname;

		// File descriptor 
		int fd;

		// Termios options for the connection.
    	struct termios options;

		// Time of last write().
		timespec lastWrite;

		// Mutex on read/write
		// Ptreads, so UNIX-only.
		pthread_mutex_t mutex;

		// Wait for the line to open in given time, a non-blocking select.
		// TODO: What was this about?
		int select(
			int microseconds = 0, // 50500
			int seconds = 0, 
			bool chkRead = false, 
			bool chkWrite = false, 
			bool chkError = false
		);

	private:

		/**
		 * Helper methods perform the actual read/write WITHOUT locking.
		 * By doing this, mutexes can be used in read(), write(), and 
		 * writeRead().
		 */
		std::string doRead(unsigned int bytes = 1000);
		bool doWrite(std::string data, bool priority = false); 

};

#endif
