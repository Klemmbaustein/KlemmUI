#pragma once
#include <vector>
#include "StringParse.h"
#include <functional>

namespace kui::parseError
{
	void SetCode(std::vector<stringParse::Line>& Code, std::string FileName);
	void SetLine(size_t Index);

	void Error(const std::string& Message, const stringParse::StringToken& From);
	void ErrorNoLine(const std::string& Message);

	int GetErrorCount();
	void ResetError();

	extern std::function<void(std::string Message, size_t Line, size_t Begin, size_t End)> ErrorCallback;
}