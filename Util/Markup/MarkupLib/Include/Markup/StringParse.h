#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <optional>

namespace KlemmUI::StringParse
{
	struct StringToken
	{
		StringToken(std::string Text, size_t BeginChar, size_t EndChar, size_t Line);
		StringToken(std::string Text, size_t BeginChar, size_t Line);
		StringToken() {}

		std::string Text;
		size_t BeginChar = 0, EndChar = 0, Line = SIZE_MAX;

		operator std::string() const
		{
			return Text;
		}

		operator std::string&()
		{
			return Text;
		}

		operator const std::string&() const
		{
			return Text;
		}

		bool operator==(const std::string& Other) const { return Text == Other; }
		bool operator==(const StringToken& Other) const { return Text == Other.Text; }

		void Add(char c, size_t NewLast = SIZE_MAX);
		bool IsName() const;
		bool Empty() const { return Text.empty(); }
		size_t Size() const { return Text.size(); }
	};

	struct Line
	{
		std::vector<StringToken> Strings;
		size_t Index = 0;

		size_t StringPos = 0;

		void ResetPos() { StringPos = 0; }
		StringToken Previous();
		StringToken Get();
		StringToken Peek();
		bool Empty() const;
		StringToken GetUntil(std::string str);
		bool Contains(std::string str) const;
		Line GetLineUntil(std::string str, bool RespectBraces = false);
		Line GetInBraces(bool SquareBraces = false);
		std::vector<Line> GetLinesInBraces();

		StringToken GetLast();
	};

	bool IsStringToken(std::string Element);
	bool IsVectorToken(std::string Element);
	bool IsSizeValue(std::string Element);
	bool Is1DSizeValue(std::string Element);
	std::string GetAlign(std::string Element);
	std::string GetBorderType(std::string Element);
	bool IsNumber(std::string Element);

	std::string ToCppCode(std::string Value);

	std::vector<Line> SeparateString(std::string String);

	std::optional<StringToken> GetTokenAt(std::vector<Line> Lines, size_t Character, size_t Line);

	struct Size
	{
		std::string SizeValue;
		std::string SizeMode;

		Size(std::string SizeString, bool Is1D = false);

		static std::string SizeModeToKUISizeMode(std::string Mode);
	};
}
