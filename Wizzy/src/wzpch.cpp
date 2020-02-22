#include "wzpch.h"

string EXECUTABLE_PATH = "";

const string& GetExecutablePath()
{
	return EXECUTABLE_PATH;
}
void SetExecutablePath(const string& p)
{
	EXECUTABLE_PATH = p;
}
