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

#include "commandparsing.h"

#include "ntregmanipulator.h"


int main(int argc, char  * argv[])
{
	if (argc <3)
		exit(1);
	std::vector<std::string> arguments = {};
	for (int i = 1;i < argc;i++)
		arguments.push_back(std::string(argv[i]));
	std::transform(arguments[0].begin(), arguments[0].end(), arguments[0].begin(), ::tolower);
	commandparsing::runcommand(argc, arguments,argv);
	return 0;
}

