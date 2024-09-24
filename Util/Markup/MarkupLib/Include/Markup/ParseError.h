#pragma once
#include <vector>
#include "StringParse.h"

namespace kui::parseError
{
	void SetCode(std::vector<stringParse::Line>& Code, std::string FileName);
	void SetLine(size_t Index);

	void Error(const std::string& Message);
	void ErrorNoLine(const std::string& Message);

	int GetErrorCount();
	void ResetError();
}