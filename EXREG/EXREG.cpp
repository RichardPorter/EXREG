// EXREG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <Winternl.h>
#include <iostream>
#include <ctype.h>

#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
void passtoreg(int argc, char * argv[] );

int main(int argc, char  * argv[])
{

	if (argc == 1)
		return 0;
	std::map<std::string, int> argumentmap = { {"delete",3} };
	std::vector<std::string> arguments = {};
	for (int i = 1;i < argc;i++)
		arguments.push_back(std::string(argv[i]));
	std::transform(arguments[0].begin(), arguments[0].end(), arguments[0].begin(), ::tolower);

	switch (argumentmap[arguments[0]]) {
	case 3: std::cout << "case 3";
		if (argc == 2){
		std::cout << "Not enough arguments" << std::endl;
		break;
		}
		if (arguments[1] != "/v" && arguments[1] != "/ve" && arguments[1] != "/va") {
		//DELETE KEY
			;
		}
		else if (arguments[1] == "/v") {
		//DELETE VALUE by name
			;
		}
		else if (arguments[1] == "ve") {
		//DELETE empty value
			;
		}
		else if (arguments[1] == "/va") {
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
/*	passtoreg(argc, argv);
	for (int i = 0;i < argc-1;i++)
		std::cout << arguments[i]<<std::endl;*/
    return 0;
}
void passtoreg(int argc, char  * argv[]) {
	std::string argument="reg.exe";
	
	for (int argn = 1; argn < argc; argn++)
	{
		argument=argument+" "+ std::string(argv[argn]);
	}
	//std::cout << argument;
	std::vector<char> command(argument.begin(), argument.end());
	command.push_back('\0');
	//printf("%s", &command[0]);
	system(&command[0]);
	return;
}

