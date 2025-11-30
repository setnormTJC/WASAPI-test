#include "MyException.h"

#include <iostream>

#include<sstream> //for ostringstream


MyException::MyException(const std::string& errorMessage, const int lineNumber, const char* filename)
	:errorMessage(errorMessage), lineNumber(lineNumber), filename(filename)
{
}

std::string MyException::whatWentWrong() const
{
	std::ostringstream oss;

	oss << errorMessage << "\n";
	oss << " occurred on line " << lineNumber << "\n";
	oss << " in file: " << filename << "\n";

	return oss.str(); //convert output string stream to string (to match "simple" return type)
}