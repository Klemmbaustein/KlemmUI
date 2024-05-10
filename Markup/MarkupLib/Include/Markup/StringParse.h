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
		bool Contains(std::string str) const;
		Line GetLineUntil(std::string str, bool RespectBraces = false);
		Line GetInBraces(bool SquareBraces = false);
		std::vector<Line> GetLinesInBraces();
	};

	bool IsStringToken(std::string Element);
	bool IsVectorToken(std::string Element);
	bool IsSizeValue(std::string Element);
	std::string GetAlign(std::string Element);
	std::string GetBorderType(std::string Element);
	bool IsNumber(std::string Element);

	std::string ToCppCode(std::string Value);

	std::vector<Line> SeparateString(std::string String);

	struct Size
	{
		std::string SizeValue;
		std::string SizeMode;

		Size(std::string SizeString);

		static std::string SizeModeToKUISizeMode(std::string Mode);
	};
}