#pragma once

#ifdef WINDOWS
#include <Windows.h>
#endif

#include <iostream>
#include <regex>


void PrintNewLine(const std::string& str)
{
	std::cout << "\n" + str;
}

void PrintWarning(const std::string& str)
{
#ifdef WINDOWS
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "\n" + str + "\n";
	SetConsoleTextAttribute(hConsole, 7);
#else
	std::cout << "\n" + str + "\n";
#endif
}

void PrintVector(const std::vector<std::string>& strings)
{
	std::string output;
	output = "[";

	if (strings.size() == 0)
	{
		output += "]";
	}
	else
	{
		std::for_each(strings.begin(), strings.end() - 1, [&output](const std::string& element) {
			output += element + ", ";
		});
		output += strings.back() + "]";
	}

	std::cout << output;
}

void PrintVector(const std::vector<unsigned int>& values)
{
	std::vector<std::string> strings;
	std::transform(values.begin(), values.end(), std::back_inserter(strings), [](const int& value) { return std::to_string(value); });

	PrintVector(strings);
}
