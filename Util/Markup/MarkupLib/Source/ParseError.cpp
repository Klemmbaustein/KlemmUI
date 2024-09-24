#include "Markup/ParseError.h"
#include <iostream>
#include <vector>
#include "Markup/StringParse.h"
using namespace kui;

namespace kui::parseError
{
	std::vector<stringParse::Line>* LoadedCode = nullptr;
	std::string ActiveFile;
	size_t LineIndex = 0;
	int ErrorCount = 0;
}

void parseError::SetCode(std::vector<stringParse::Line>& Code, std::string FileName)
{
	LoadedCode = &Code;
	ActiveFile = FileName;
}

void parseError::SetLine(size_t Index)
{
	LineIndex = Index;
}

void parseError::Error(const std::string& Message)
{
	auto& Line = LoadedCode->at(LineIndex);
	std::cerr << ActiveFile << ":" << Line.Index + 1 << ": Error: " << Message << std::endl;
	std::string LineString;
	for (auto& i : Line.Strings)
	{
		if (i == "," || i == "(" || i == ")")
		{
			LineString.pop_back();
		}
		LineString.append(i);
		if (i != "(" && i != ")")
		{
			LineString.append(" ");
		}
	}
	std::cerr << LineString << std::endl;
	for (size_t i = 0; i < LineString.size(); i++)
	{
		std::cerr << '^';
	}
	std::cerr << std::endl;
	ErrorCount++;
}

void kui::parseError::ErrorNoLine(const std::string& Message)
{
	std::cerr << ActiveFile << ": Error: " << Message << std::endl;
	ErrorCount++;
}

int parseError::GetErrorCount()
{
	return ErrorCount;
}

void parseError::ResetError()
{
	ErrorCount = 0;
}
