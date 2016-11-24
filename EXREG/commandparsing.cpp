#include "stdafx.h"
#include "commandparsing.h"
#include "stdafx.h"


#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

#include "ntregmanipulator.h"

void passtoreg(int argc, char * argv[]);
commandparsing::commandparsing()
{
	

}


commandparsing::~commandparsing()
{
}
int commandparsing::runcommand(int argc, std::vector<std::string> arguments, char * argv[])
{
	int commandresult;
	ntregmanipulator ntreg = ntregmanipulator::ntregmanipulator();
	UNICODE_STRING key_name;
	UNICODE_STRING value_name;
	std::map<std::string, int> argumentmap = { { "delete",3 } };
	switch (argumentmap[arguments[0]]) {
	case 3: std::cout << "case 3";
		if (argc == 2) {
			std::cout << "Not enough arguments" << std::endl;
			break;
		}
		if (argc==3||(arguments[2] != "/v" && arguments[2] != "/ve" && arguments[2] != "/va")) {
			key_name = ntreg.unescapestring(arguments[1]);
			commandresult=ntreg.DeleteKeyByName(key_name);
			exit(commandresult);
		}
		else if (arguments[2] == "/v") {
			//DELETE VALUE by name
			;
		}
		else if (arguments[2] == "ve") {
			//DELETE empty value
			;
		}
		else if (arguments[2] == "/va") {
			//delete key
			//create key
			;
		}
		else {
			std::cout << "Invalid argument";
		}
		break;
	default:  passtoreg(argc, argv);
		break;
	}
}


void passtoreg(int argc, char  * argv[]) {
	std::string argument = "reg.exe";

	for (int argn = 1; argn < argc; argn++)
	{
		argument = argument + " " + std::string(argv[argn]);
	}
	//std::cout << argument;
	std::vector<char> command(argument.begin(), argument.end());
	command.push_back('\0');
	//printf("%s", &command[0]);
	system(&command[0]);
	return;
}