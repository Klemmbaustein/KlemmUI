#include "Markup/ParseError.h"
#include <iostream>
#include <vector>
#include "Markup/StringParse.h"
using namespace KlemmUI;

namespace KlemmUI::ParseError
{
	std::vector<StringParse::Line>* LoadedCode = nullptr;
	size_t LineIndex = 0;
	int ErrorCount = 0;
}

void ParseError::SetCode(std::vector<StringParse::Line>& Code)
{
	LoadedCode = &Code;
}

void ParseError::SetLine(size_t Index)
{
	LineIndex = Index;
}

void ParseError::Error(const std::string& Message)
{
	auto& Line = LoadedCode->at(LineIndex);
	std::cout << "Error parsing line " << Line.Index + 1 << ": " << Message << std::endl;
	size_t Length = 0;
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
	std::cout << LineString << std::endl;
	for (size_t i = 0; i < LineString.size(); i++)
	{
		std::cout << '^';
	}
	std::cout << std::endl;
	ErrorCount++;
}

int ParseError::GetErrorCount()
{
	return ErrorCount;
}

void ParseError::ResetError()
{
	ErrorCount = 0;
}
