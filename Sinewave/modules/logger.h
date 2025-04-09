#pragma once
#include <iostream>
#include <windows.h>

class Logger
{
public:
	enum type {
		INFO,
		SUCCESS,
		ERR,
	};

	static void log(type t, const std::string& message);
};

