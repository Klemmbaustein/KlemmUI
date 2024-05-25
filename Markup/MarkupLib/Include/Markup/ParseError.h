#pragma once
#include <vector>
#include "StringParse.h"

namespace KlemmUI::ParseError
{
	void SetCode(std::vector<StringParse::Line>& Code);
	void SetLine(size_t Index);

	void Error(const std::string& Message);
	void ErrorNoLine(const std::string& Message);

	int GetErrorCount();
	void ResetError();
}