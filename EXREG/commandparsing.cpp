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
#include <regex>
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
	UNICODE_STRING value_data;
	DWORD value_type = REG_SZ;
	DWORD data_dword;
	UINT64 data_qword;
	bool recurse = false;
	int value_data_position = 0;
	int argnum;
	std::map<std::string, int> regtypemap = { { "REG_SZ",REG_SZ },{ "REG_MULTI_SZ",REG_MULTI_SZ },{ "REG_RESOURCE_REQUIREMENTS_LIST",REG_RESOURCE_REQUIREMENTS_LIST },{ "REG_DWORD",REG_DWORD },{ "REG_BINARY",REG_BINARY },{ "REG_DWORD_LITTLE_ENDIAN",REG_DWORD_LITTLE_ENDIAN },{ "REG_DWORD_BIG_ENDIAN",REG_DWORD_BIG_ENDIAN },{ "REG_NONE",REG_NONE },{ "REG_RESOURCE_LIST",REG_RESOURCE_LIST },{ "REG_FULL_RESOURCE_DESCRIPTOR",REG_FULL_RESOURCE_DESCRIPTOR },{ "REG_QWORD",REG_QWORD },{ "REG_EXPAND_SZ",REG_EXPAND_SZ } };
	std::map<std::string, int> argumentmap = { { "add",2 }, { "delete",3 },{"query",4 } };
	std::string reg_sz_separator = "\\0";
	bool recursequery = false;
	switch (argumentmap[arguments[0]]) {
	case 2: std::cout << "case2" << std::endl;
		key_name = ntreg.unescapestring(arguments[1]);
		if (argc == 3 || (arguments[2] != "/v" && arguments[2] != "/ve")) {

			commandresult = ntreg.CreateKey(key_name);
			exit(commandresult);
		}
		else {
			value_name.Length = 0;
			argnum = 3;
			if (arguments[2] == "/v") {
				value_name = ntreg.unescapestring(arguments[3]);
				argnum++;
			}

			while (argnum < argc - 1) {
				if (arguments[argnum] == "/t") {
					std::transform(arguments[argnum + 1].begin(), arguments[argnum + 1].end(), arguments[argnum + 1].begin(), ::toupper);
					value_type = regtypemap[arguments[argnum + 1]];
					argnum++;
				}
				if (arguments[argnum] == "/s") {
					reg_sz_separator = arguments[argnum + 1];
					argnum++;
				}
				if (arguments[argnum] == "/d") {
					value_data_position = argnum + 1;
					argnum++;
				}
				argnum++;
			}
			if (value_data_position == 0) {
				commandresult = ntreg.SetKeyValue(key_name, value_name, (ULONG)value_type, 0, NULL);
				exit(commandresult);
			}
			if (value_type == REG_DWORD || value_type == REG_DWORD_LITTLE_ENDIAN || value_type == REG_DWORD_BIG_ENDIAN) {
				data_dword = (DWORD)std::stoul(arguments[value_data_position], NULL, 0);
				commandresult = ntreg.SetKeyValue(key_name, value_name, (ULONG)value_type, sizeof(DWORD), &data_dword);
				exit(commandresult);
			}
			else if (value_type == REG_QWORD || value_type == REG_QWORD_LITTLE_ENDIAN) {
				data_qword = (UINT64)std::stoul(arguments[value_data_position], NULL, 0);
				commandresult = ntreg.SetKeyValue(key_name, value_name, (ULONG)value_type, sizeof(UINT64), &data_qword);
				exit(commandresult);
			}
			else if (value_type == REG_MULTI_SZ || value_type == REG_EXPAND_SZ || value_type == REG_SZ) {

				if (value_type == REG_MULTI_SZ) {
					arguments[value_data_position] = (arguments[value_data_position].replace(arguments[value_data_position].find(reg_sz_separator), reg_sz_separator.length(), "*0000"));
					arguments[value_data_position] = arguments[value_data_position] + "*0000";
				}
				arguments[value_data_position] = arguments[value_data_position] + "*0000";
				value_data = ntreg.unescapestring(arguments[value_data_position]);
				commandresult = ntreg.SetKeyValue(key_name, value_name, (ULONG)value_type, value_data.Length, value_data.Buffer);
				exit(commandresult);
			}
			else {
				value_data = ntreg.unescapestring(arguments[value_data_position]);
				commandresult = ntreg.SetKeyValue(key_name, value_name, (ULONG)value_type, value_data.Length, value_data.Buffer);
				exit(commandresult);
			}

		}
		break;
	case 3: std::cout << "case 3";
		if (argc == 2) {
			std::cout << "Not enough arguments" << std::endl;
			break;
		}
		if (argc == 3 || (arguments[2] != "/v" && arguments[2] != "/ve" && arguments[2] != "/va")) {
			key_name = ntreg.unescapestring(arguments[1]);
			commandresult = ntreg.DeleteKeyByName(key_name);
			exit(commandresult);
		}
		else if (arguments[2] == "/v" && argc >= 4) {
			key_name = ntreg.unescapestring(arguments[1]);
			value_name = ntreg.unescapestring(arguments[3]);
			ntreg.DeleteKeyValueByName(key_name, value_name);
		}
		else if (arguments[2] == "/ve") {
			key_name = ntreg.unescapestring(arguments[1]);
			value_name.Length = 0;
			ntreg.DeleteKeyValueByName(key_name, value_name);
		}
		else if (arguments[2] == "/va") {
			//delete key
			key_name = ntreg.unescapestring(arguments[1]);
			commandresult = ntreg.DeleteKeyByName(key_name);
			//create key
			commandresult = ntreg.CreateKey(key_name);
		}
		else {
			std::cout << "Invalid argument";
		}
		break;
	case 4: key_name = ntreg.unescapestring(arguments[1]);
		argnum = 1;
		if (argc == 3 || (arguments[2] != "/v" && arguments[2] != "/ve")) {
		key_name = ntreg.unescapestring(arguments[1]);
		while (argnum < argc - 1) {
				if (arguments[argnum] == "/s") {
				recurse=true;
				
			}
				argnum++;
		
		}
		ntreg.QueryKeyByName(key_name,recurse);
		}
		else if ((arguments[2] == "/v"&&argc > 4)|| (arguments[2] == "/ve"&&argc > 3)) {
			
			key_name = ntreg.unescapestring(arguments[1]);
			if (arguments[2] == "/v"&&argc > 4) {
				value_name = ntreg.unescapestring(arguments[3]);
			}
			else { value_name.Length = 0; }
			std::string value_query_result = ntreg.QueryKeyValueByName(key_name, value_name);
			if (value_query_result != "") {
				std::cout << ntreg.escapestring(key_name)<<std::endl;
				std::cout << value_query_result;

			}
		}
		else if (arguments[2] == "/ve"&&argc > 4) {}
		else {
			std::cout << "Syntax Error" << std::endl;
			exit(1);
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