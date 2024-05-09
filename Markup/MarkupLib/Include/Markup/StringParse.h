#pragma once
#include <vector>
#include <string>

namespace KlemmUI::StringParse
{
	struct Line
	{
		std::vector<std::string> Strings;
		size_t Index = 0;

		size_t StringPos = 0;

		std::string Previous();
		std::string Get();
		std::string Peek();
		bool Empty() const;
		std::string GetUntil(std::string str);
		Line GetLineUntil(std::string str, bool RespectBraces = false);
		Line GetInBraces(bool SquareBraces = false);
		std::vector<Line> GetLinesInBraces();
	};

	std::vector<Line> SeparateString(std::string String);
}