#include "Command.hpp"
#include "picojson.h" // Nice library on Github

#include <string>
#include <iostream>
#include <sstream>

Command::Command(std::string cmd):
	type(COMMAND_NONE)
{
	parseJson(cmd);
}

bool Command::operator==(const Command& cmd) const
{
	if(type != cmd.type) {
		return false;
	}

	if(params.size() != cmd.params.size()) {
		return false;
	}

	for(unsigned int i = 0; i < params.size(); i++) {
		if(params[i] != cmd.params[i]) {
			return false;
		}
	}
	return true;
}

void Command::parseJson(std::string json)
{
	// FIXME: References and const correctness
	// FIXME: String bleh. 
	picojson::value root;
	const char* base = 0;
	std::string err;
	std::string str;
	picojson::value val;
	picojson::array arr;
	double dbl;

	/**
	 * 0) Convert into JSON objects.
	 * Nasty boilerplate.
	 */
	base = json.c_str();

	picojson::parse(root, base, base + strlen(base), &err);

	if(!err.empty()) {
		std::cerr << err << std::endl;
		return;
	}

	if(!root.is<picojson::object>()) {
		std::cerr << "Not a JSON object." << std::endl;
		return;
	}

	const picojson::object& obj = root.get<picojson::object>();

	/**
	 * 1) Extract command type. (TODO)
	 * This is absolutely required. 
	 */
	if(!obj.count("cmd")) {
		std::cerr << "No command in JSON payload." << std::endl;
		return;
	}

	val = obj.find("cmd")->second;

	if(!val.is<std::string>()) {
		std::cerr << "JSON command not a string." << std::endl;
		return;
	}

	str = val.get<std::string>();

	if(!str.size()) {
		type = COMMAND_NONE;
	} 
	else if(str == "motor") {
		type = COMMAND_MOTOR;
	} 
	else if(str == "blink") {
		type = COMMAND_BLINK;
	} 
	else if(str == "blinkOne") {
		type = COMMAND_BLINK_ONE;
	} 
	else if(str == "stop") {
		type = COMMAND_STOP;
	}
	else if(str == "w") {
		type = COMMAND_SIMPLE_W;
	}
	else if(str == "a") {
		type = COMMAND_SIMPLE_A;
	}
	else if(str == "s") {
		type = COMMAND_SIMPLE_S;
	}
	else if(str == "d") {
		type = COMMAND_SIMPLE_D;
	}
	else if(str == "e") {
		type = COMMAND_SIMPLE_E;
	}

	/**
	 * 2) Extract Parameters.
	 * Parameters are completely optional and only applicable
	 * to some command types.
	 */
	if(obj.count("params")) 
	{
		val = obj.find("params")->second;

		if(val.is<picojson::array>()) {

			arr = val.get<picojson::array>();

			for(unsigned int i = 0; i < arr.size(); i++) {
				if(!arr[i].is<double>()) {
					break;
				}
				dbl = arr[i].get<double>();
				params.push_back(static_cast<int>(dbl));
			}
		}
	}
}
