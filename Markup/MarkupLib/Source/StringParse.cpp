#include "Markup/StringParse.h"
#include "Markup/ParseError.h"
#include <unordered_set>
#include <iostream>
#include <cstring>
#include <sstream>
using namespace KlemmUI;

static std::unordered_set<char> Whitespace =
{
	' ',
	'\t',
	'\n',
	'\r',
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

static std::string RemoveComments(std::string Code)
{
	std::stringstream StrStream = std::stringstream(Code);
	std::string OutCode;

	char last = 0;

	bool MultiLineComment = false;
	while (true)
	{

		if (StrStream.eof())
		{
			break;
		}

		char Buffer[8192];
		StrStream.getline(Buffer, sizeof(Buffer));

		for (char c : Buffer)
		{
			if (last == c && c == '/')
			{
				OutCode.pop_back();
				break;
			}

			if (last == '/' && c == '*')
			{
				MultiLineComment = true;
				OutCode.pop_back();
				last = 0;
			}

			if (last == '*' && c == '/')
			{
				MultiLineComment = false;
				continue;
			}

			if (c == 0)
			{
				OutCode.push_back('\n');
				last = 0;
				break;
			}

			last = c;

			if (!MultiLineComment)
			{
				OutCode.push_back(c);
			}
		}
	}
	return OutCode;
}

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

	if (Element.size() > 2 && Element[0] == '$')
	{
		return IsStringToken(Element.substr(1));
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

bool KlemmUI::StringParse::Is1DSizeValue(std::string Element)
{
	return !Size(Element, true).SizeValue.empty();
}

std::string KlemmUI::StringParse::GetAlign(std::string Element)
{
	if (Element == "default")
	{
		return "KlemmUI::UIBox::Align::Default";
	}
	if (Element == "reverse")
	{
		return "KlemmUI::UIBox::Align::Reverse";
	}
	if (Element == "centered")
	{
		return "KlemmUI::UIBox::Align::Centered";
	}
	return "";
}

std::string KlemmUI::StringParse::GetBorderType(std::string Element)
{
	if (Element == "none")
	{
		return "KlemmUI::UIBox::BorderType::None";
	}
	if (Element == "rounded")
	{
		return "KlemmUI::UIBox::BorderType::Rounded";
	}
	if (Element == "border")
	{
		return "KlemmUI::UIBox::BorderType::DarkenedEdge";
	}
	return "";
}

bool KlemmUI::StringParse::IsNumber(std::string Element)
{
	char* p;
	return(strspn(Element.c_str(), "-.0123456789") == Element.size());
	return !(*p);

}

std::string KlemmUI::StringParse::ToCppCode(std::string Value)
{
	if (IsNumber(Value))
	{
		return "float(" + Value + ")";
	}
	// (number, number, number) or maybe just (number)
	if (IsVectorToken(Value))
	{
		auto Values = StringParse::SeparateString(Value);

		auto& Strings = Values[0].Strings;

		// remove the first '('
		Strings.erase(Strings.begin());
		// remove the last ')'
		Strings.pop_back();
		
		std::string OutString;

		std::string Value;

		for (auto& i : Strings)
		{
			if (i == ",")
			{
				OutString.append(", ");
			}
			else if (i != "-")
			{
				Value += i;
				OutString.append("float(" + Value + ")");
				Value.clear();
			}
			else
			{
				Value += i;
			}
		}
		return OutString;
	}
	if (IsSizeValue(Value))
	{
		return ToCppCode(Size(Value).SizeValue);
	}
	if (IsStringToken(Value))
	{
		if (Value[0] == '$')
		{
			return "KlemmUI::MarkupLanguageManager::GetActive()->GetString(" + Value.substr(1) + ")";
		}
		return Value;
	}

	return Value;
}

KlemmUI::StringParse::Size::Size(std::string SizeString, bool Is1D)
{
	// Just a vector/number.
	if ((!Is1D && IsVectorToken(SizeString)) || IsNumber(SizeString))
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
	if ((!Is1D && !IsVectorToken(Value)) || (Is1D && !IsNumber(Value)))
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
	String = RemoveComments(String);
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
	{
		Lines.push_back(CurrentLine);
	}
	return Lines;
}
