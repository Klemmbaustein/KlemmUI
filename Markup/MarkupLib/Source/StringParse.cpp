#include "Markup/StringParse.h"
#include "Markup/ParseError.h"
#include <unordered_set>
#include <iostream>
using namespace KlemmUI;

static std::unordered_set<char> Whitespace =
{
	' ',
	'\t',
	'\n'
};

static std::unordered_set<char> SpecialChars =
{
	'<',
	'>',
	'(',
	')',
	'=',
	',',
	'+',
	'-',
	'*',
	'/',
	'%',
	'[',
	']'
};

std::string StringParse::Line::Previous()
{
	if (StringPos <= Strings.size() && StringPos > 0)
	{
		return Strings[StringPos - 1];
	}
	return "";
}

std::string StringParse::Line::Get()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos++];
	}
	return "";
}

std::string StringParse::Line::Peek()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos];
	}
	return "";
}

std::string StringParse::Line::GetUntil(std::string str)
{
	std::string Condition;
	while (!Empty())
	{
		std::string Next = Get();
		if (Next == str)
		{
			break;
		}
		Condition.append(Next);
	}
	return Condition;
}

bool KlemmUI::StringParse::Line::Contains(std::string str) const
{
	for (auto& i : Strings)
	{
		if (i == str)
		{
			return true;
		}
	}
	return false;
}

StringParse::Line StringParse::Line::GetLineUntil(std::string str, bool RespectBraces)
{
	uint8_t Depth = 1;
	Line Condition;
	while (!Empty())
	{
		std::string Next = Get();
		if (Next == "(")
		{
			Depth++;
		}

		if (Next == ")")
		{
			Depth--;
			if (Depth == 0)
			{
				break;
			}
		}

		if (Next == str && (!RespectBraces || Depth == 1))
		{
			break;
		}
		Condition.Strings.push_back(Next);
	}
	return Condition;
}

StringParse::Line StringParse::Line::GetInBraces(bool SquareBraces)
{
	uint8_t Depth = 1;
	Line Condition;
	bool Success = false;
	while (!Empty())
	{
		std::string Next = Get();

		if (Next == "(" || (SquareBraces && Next == "["))
		{
			Depth++;
		}

		if (Next == ")" || (SquareBraces && Next == "]"))
		{
			Depth--;
			if (Depth == 0)
			{
				Success = true;
				break;
			}
		}
		Condition.Strings.push_back(Next);
	}
	if (!Success)
	{
		if (SquareBraces)
		{
			ParseError::Error("Expected a matching ']'");
		}
		else
		{
			ParseError::Error("Expected a matching ')'");
		}
	}
	return Condition;
}

std::vector<StringParse::Line> StringParse::Line::GetLinesInBraces()
{
	uint8_t Depth = 1;
	std::vector<Line> Conditions = { };
	bool Success = false;
	while (!Empty())
	{
		std::string Next = Get();

		if (Next == "(")
		{
			Depth++;
		}

		if (Next == ")")
		{
			Depth--;
			if (Depth == 0)
			{
				Success = true;
				break;
			}
		}

		if (Next == "," && Depth == 1)
		{
			Conditions.push_back(Line());
			continue;
		}

		if (!Conditions.size())
		{
			Conditions.push_back(Line());
		}
		Conditions[Conditions.size() - 1].Strings.push_back(Next);
	}
	if (!Success)
	{
		ParseError::Error("Expected a matching ')'");
	}
	return Conditions;
}

bool StringParse::Line::Empty() const
{
	return Strings.size() <= StringPos;
}

static void ParseWord(std::string& CurrentWord, StringParse::Line& CurrentLine)
{
	using namespace StringParse;

	if (!CurrentWord.empty())
	{
		CurrentLine.Strings.push_back(CurrentWord);
	}
	CurrentWord.clear();
}

bool KlemmUI::StringParse::IsStringToken(std::string Element)
{
	if (Element.size() < 2)
	{
		return false;
	}
	return Element[0] == '"' && Element[Element.size() - 1] == '"';
}

bool KlemmUI::StringParse::IsVectorToken(std::string Element)
{
	if (IsNumber(Element))
	{
		return true;
	}

	if (Element.size() < 2)
	{
		return false;
	}
	return Element[0] == '(' && Element[Element.size() - 1] == ')';
}

bool KlemmUI::StringParse::IsSizeValue(std::string Element)
{
	return !Size(Element).SizeValue.empty();
}

bool KlemmUI::StringParse::IsNumber(std::string Element)
{
	return !Element.empty() && std::find_if(Element.begin(), Element.end(),
		[](unsigned char c) { return !std::isdigit(c); }) == Element.end();
}

std::string KlemmUI::StringParse::ToCppCode(std::string Value)
{
	if (IsNumber(Value))
	{
		return Value;
	}
	if (IsVectorToken(Value))
	{
		return Value.substr(1, Value.size() - 2);
	}
	if (IsSizeValue(Value))
	{
		return ToCppCode(Size(Value).SizeValue);
	}
	if (IsStringToken(Value))
	{
		return Value;
	}

	return Value;
}

KlemmUI::StringParse::Size::Size(std::string SizeString)
{
	// Just a vector.
	if (IsVectorToken(SizeString))
	{
		SizeValue = SizeString;
		SizeMode.clear();
	}
	if (SizeString.size() <= 2)
	{
		// Size suffix is always 2 characters. If there's only 2 characters, there is either no valid size suffix
		// or no number attached to it.
		return;
	}

	std::string SizeSuffix = SizeString.substr(SizeString.size() - 2);

	std::string Value = SizeString.substr(0, SizeString.size() - 2);

	// Value before size suffix is not a size.
	if (!IsVectorToken(Value))
	{
		return;
	}

	// Pixel Relative
	if (SizeSuffix == "px" || SizeSuffix == "pr")
	{
		SizeValue = Value;
		SizeMode = "pr";
		return;
	}
	// Aspect Relative
	if (SizeSuffix == "ar")
	{
		SizeValue = Value;
		SizeMode = "ar";
		return;
	}
	// Screen Relative (default)
	if (SizeSuffix == "sr")
	{
		SizeValue = Value;
		SizeMode.clear();
		return;
	}
}

std::string KlemmUI::StringParse::Size::SizeModeToKUISizeMode(std::string Mode)
{
	if (Mode == "pr" || Mode == "px")
	{
		return "KlemmUI::UIBox::SizeMode::PixelRelative";
	}
	if (Mode == "ar")
	{
		return "KlemmUI::UIBox::SizeMode::AspectRelative";
	}
	return "KlemmUI::UIBox::SizeMode::ScreenRelative";
}

std::vector<StringParse::Line> StringParse::SeparateString(std::string String)
{
	std::vector<Line> Lines;
	Line CurrentLine;
	CurrentLine.Index = 1;
	std::string CurrentWord;
	size_t LineIndex = 0;
	size_t LineBeginIndex = SIZE_MAX;

	bool InQuotes = false;

	for (char c : String)
	{
		if (c == '\n')
		{
			LineIndex++;
		}

		if (c == '"')
		{
			if (InQuotes)
			{
				CurrentWord.push_back(c);
				ParseWord(CurrentWord, CurrentLine);
				InQuotes = false;
			}
			else
			{
				ParseWord(CurrentWord, CurrentLine);
				InQuotes = true;
				CurrentWord.push_back(c);
			}
			continue;
		}

		if (InQuotes)
		{
			CurrentWord.push_back(c);
			continue;
		}

		if (c == ';' || c == '}' || c == '{')
		{
			if (c == '{')
			{
				ParseWord(CurrentWord, CurrentLine);
				CurrentWord = { c };
				ParseWord(CurrentWord, CurrentLine);
			}

			CurrentLine.Index = LineBeginIndex;
			LineBeginIndex = SIZE_MAX;
			if (!CurrentWord.empty())
			{
				CurrentLine.Strings.push_back(CurrentWord);
			}
			if (!CurrentLine.Strings.empty())
			{
				Lines.push_back(CurrentLine);
			}

			CurrentLine = Line();
			CurrentWord.clear();
			if (c == '}')
			{
				Line l;
				l.Index = LineIndex;
				l.Strings = { { c } };
				Lines.push_back(l);
			}
		}
		else if (Whitespace.contains(c))
		{
			ParseWord(CurrentWord, CurrentLine);
		}
		else if (SpecialChars.contains(c))
		{
			ParseWord(CurrentWord, CurrentLine);
			CurrentWord = { c };
			ParseWord(CurrentWord, CurrentLine);
			if (LineBeginIndex == SIZE_MAX)
				LineBeginIndex = LineIndex;
		}
		else
		{
			if (LineBeginIndex == SIZE_MAX)
				LineBeginIndex = LineIndex;
			CurrentWord.push_back(c);
		}
	}
	ParseWord(CurrentWord, CurrentLine);

	if (!CurrentLine.Strings.empty())
		Lines.push_back(CurrentLine);	{

	}
	return Lines;
}
