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

void parseError::Error(const std::string& Message, const stringParse::StringToken& From)
{
	ErrorCallback(Message, From.Line, From.BeginChar, From.EndChar);
	ErrorCount++;
}

static size_t GetLineIndexFromLineNumber(size_t Number, size_t Begin)
{
	using namespace kui::parseError;
	for (size_t i = 0; i < LoadedCode->size(); i++)
	{
		for (auto& Token : LoadedCode->at(i).Strings)
		{
			if (Token.Line == Number && Token.BeginChar == Begin)
			{
				return i;
			}
		}
	}
	return LineIndex;
}

static void PrintError(std::string Message, size_t Line, size_t Begin, size_t End)
{
	using namespace parseError;
	auto& LineContent = LoadedCode->at(GetLineIndexFromLineNumber(Line, Begin));
	std::cerr << ActiveFile << "(" << Line + 1 << "," << Begin + 1 << ") Error: " << Message << std::endl;
	std::string LineString;
	for (auto& i : LineContent.Strings)
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

	std::string LineNumber = std::to_string(Line + 1) + ":";
	LineNumber.resize(6, ' ');

	std::cerr << LineNumber << LineString << std::endl;
	std::cerr << "      ";
	for (size_t i = 0; i < LineString.size(); i++)
	{
		std::cerr << '^';
	}
	std::cerr << std::endl;
}

std::function<void(std::string Message, size_t Line, size_t Begin, size_t End)> parseError::ErrorCallback = &PrintError;

void parseError::ErrorNoLine(const std::string& Message)
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
