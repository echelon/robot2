#ifndef ROBOT2_Command
#define ROBOT2_Command

#include <string>
#include <vector>

/**
 * All of the command types.
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

class Command
{

	public:
		/**
		 * CTOR : Empty.
		 */
		Command() {};

		/**
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
		 * Test for equality.
		 * TODO: Must test parameters too!
		 */
		bool operator==(const Command& cmd) const;

	private:

		CommandType type;

		std::vector<int> params;

		void parseJson(std::string json);
};

#endif
