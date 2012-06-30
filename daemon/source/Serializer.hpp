#ifndef ROBOT2_Serializer
#define ROBOT2_Serializer

#include <string>
//class Serial;
#include "Serial.hpp"

class Serializer
{

	public:
		/**
		 * CTOR
		 * Give serializer access to the serial object.
		 * Serialzer doesn't own Serial.
		 */
		Serializer(Serial* ser): serial(ser) {};

		/**
		 * Send a raw command to the serial device.
		 * A non-blocking call.
		 * TODO: Auto <CR> insert?
		 */
		void sendRaw(std::string cmd) { serial->write(cmd); };

		/**
		 * Set two leds to blink.
		 * Rate can be between 0 and 127. 
		 * A non-blocking call.
		 * TODO/FIXME: TEST
		 */
		void blink(unsigned int rate1, unsigned int rate2);

		/**
		 * Set a single led to blink.
		 * Rate can be between 0 and 127. 
		 * A non-blocking call.
		 * TODO/FIXME: TEST
		 */
		void blinkLed(unsigned int ledId, unsigned int rate);

		/**
		 * Set the motor velocities.
		 * Positive and negative set to different directions. 
		 * TODO: See VPID settings to determine max velocity
		 *
		 * VIPID sets
		 * 	 * Proportional, Integral, Derivative, and Loop (PIDL)
		 * 	 vpid 10:0:0:700 -- sets p, i, d, l respectively
		 * 	 vpid -- returns the values. 
		 */
		void motor(int vel1, int vel2);

		/**
		 * Immediately stops the motors.
		 * Non-blocking.
		 */
		void stop();

		/**
		 * Get the firmware version. 
		 * A blocking call. 
		 */
		//std::string getFirmware();

		/**
		 * Reset the Serializer device.
		 * A blocking call.
		 */
		//void reset();

		/**
		 * DIGO -- travel X direction for Y time
		 * 	Requires VPID and DPID to be set
		 */

		// XXX: pwm!! this ramps up motor speed!
		// XXX: Doesn't rely on PID. 
		//

		/**
		 * Get the velocities of the motors.
		 * A blocking call.
		 */
		//int[] getVelocities(); // TODO
	
	private:
		/**
		 * Pointer to the serial line object.
		 * Doesn't own it (for now!)
		 */
		Serial* serial;


};

#endif
