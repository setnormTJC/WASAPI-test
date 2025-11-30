#pragma once
#include <string>

#pragma warning(disable : 4820)
class MyException
{
private:
	std::string errorMessage{};
	int lineNumber{};
	const char* filename{};
public:
	MyException(const std::string& errorMessage, const int lineNumber, const char* filename);

	std::string whatWentWrong() const;
};