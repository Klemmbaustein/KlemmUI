#pragma once
#include <vector>
#include "StringParse.h"
#include <functional>

namespace KlemmUI::ParseError
{
	void SetCode(std::vector<StringParse::Line>& Code, std::string FileName);
	void SetLine(size_t Index);

	void Error(const std::string& Message, const StringParse::StringToken& From);
	void ErrorNoLine(const std::string& Message);

	int GetErrorCount();
	void ResetError();

	extern std::function<void(std::string Message, size_t Line, size_t Begin, size_t End)> ErrorCallback;
}