#ifndef ROBOT2_Command
#define ROBOT2_Command

/**
 * Copyright (c) 2012 Brandon Thomas
 * http://possibilistic.org | echelon@gmail.com
 * See README.md for license information. 
 *
 * Description
 * 
 * 		Command
 * This class represents commands that are sent over the wire via ZeroMQ or 
 * some other messaging system. The only objective of this class is to 
 * parse commands and make their command type and parameters available to the 
 * client. This class knows nothing about robot specifics or any kind of 
 * instruction set or other low-level robot protocol. 
 *
 * Commands may be encoded as very simple strings or JSON objects.  
 * 
 * TODO: This class is a mess. Clean it up. 
 * TODO: Documentation on JSON protocol and simple string protocol. 
 */

#include <string>
#include <vector>

/**
 * All of the command types.
 * FIXME: Some are not yet fully supported (or supported at all)
 */
enum CommandType
{
	COMMAND_NONE,

	// Primary commands 
	COMMAND_MOTOR,
	COMMAND_BLINK,
	COMMAND_BLINK_ONE,
	COMMAND_STOP,

	// Shortcut commands
	// Move motors at precompiled speeds
	COMMAND_SIMPLE_W,
	COMMAND_SIMPLE_A,
	COMMAND_SIMPLE_S,
	COMMAND_SIMPLE_D,
	COMMAND_SIMPLE_E
};

// TODO: Const-correctness

/**
 * The command class.
 */
class Command
{

	public:
		/**
		 * CTOR
		 * Empty command. 
		 */
		Command() {};

		/**
		 * CTOR
		 * Parse a JSON string into a command.
		 */
		Command(std::string cmd);

		/**
		 * Get the command type.
		 */
		CommandType getType() { return type; };

		/**
		 * Get the params.
		 * Returns an empty vector if none. 
		 */
		std::vector<int> getParams() { return params; };
	
		/**
		 * Get the properly-formatted Serializer instruction given
		 * the current Command state. Automatically inserts <CR>
		 */
		std::string getInstruction();

		/**
		 * Compare two commands. 
		 * Considers command type and all parameters. 
		 */
		bool operator==(const Command& cmd) const;

	private:

		/**
		 * The type of instruction.
		 */
		CommandType type;

		/**
		 * Only integer parameters are supported as of now.
		 */
		std::vector<int> params;

		/**
		 * Helper method to parse JSON commands. 
		 * Uses nanojson library.
		 */
		void parseJson(std::string json);
};

#endif
