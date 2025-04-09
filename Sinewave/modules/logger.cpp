#include "logger.h"

void Logger::log(type t, const std::string& message) {
	switch (t) {
	case Logger::type::SUCCESS:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		std::cout << "[SINEWAVE]: " << message << "\n";
		break;
	case Logger::type::INFO:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		std::cout << "[SINEWAVE]: " << message << "\n";
		break;
	case Logger::type::ERR:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		std::cout << "[SINEWAVE]: " << message << "\n";
	}
}
