#pragma once

#ifdef WINDOWS
#include <Windows.h>
#endif

#include <iostream>


void PrintLine(const std::string& str)
{
	std::cout << "\n" + str;
}

void PrintWarning(const std::string& str)
{
#ifdef WINDOWS
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "\n " + str + "\n\n";
	SetConsoleTextAttribute(hConsole, 7);
#else
	std::cout << "\n " + str + "\n\n";
#endif
}

