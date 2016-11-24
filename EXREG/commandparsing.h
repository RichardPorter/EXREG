#pragma once
#include "stdafx.h"

#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
class commandparsing
{
public:
	static int runcommand(int argc, std::vector<std::string> arguments, char * argv[]);
	commandparsing();
	~commandparsing();
};

