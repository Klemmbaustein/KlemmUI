#include "Markup/StringParse.h"
#include "Markup/ParseError.h"
#include <unordered_set>
#include <iostream>
#include <cstring>
#include <sstream>
using namespace kui;

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

std::string stringParse::Line::Previous()
{
	if (StringPos <= Strings.size() && StringPos > 0)
	{
		return Strings[StringPos - 1];
	}
	return "";
}

std::string stringParse::Line::Get()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos++];
	}
	return "";
}

std::string stringParse::Line::Peek()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos];
	}
	return "";
}

std::string stringParse::Line::GetUntil(std::string str)
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

bool kui::stringParse::Line::Contains(std::string str) const
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

stringParse::Line stringParse::Line::GetLineUntil(std::string str, bool RespectBraces)
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

stringParse::Line stringParse::Line::GetInBraces(bool SquareBraces)
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

std::vector<stringParse::Line> stringParse::Line::GetLinesInBraces()
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

bool stringParse::Line::Empty() const
{
	return Strings.size() <= StringPos;
}

static void ParseWord(std::string& CurrentWord, stringParse::Line& CurrentLine)
{
	using namespace stringParse;

	if (!CurrentWord.empty())
	{
		CurrentLine.Strings.push_back(CurrentWord);
	}
	CurrentWord.clear();
}

bool kui::stringParse::IsStringToken(std::string Element)
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

bool kui::stringParse::IsVectorToken(std::string Element)
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

bool kui::stringParse::IsSizeValue(std::string Element)
{
	return !Size(Element).SizeValue.empty();
}

bool kui::stringParse::Is1DSizeValue(std::string Element)
{
	return !Size(Element, true).SizeValue.empty();
}

std::string kui::stringParse::GetAlign(std::string Element)
{
	if (Element == "default")
	{
		return "kui::UIBox::Align::Default";
	}
	if (Element == "reverse")
	{
		return "kui::UIBox::Align::Reverse";
	}
	if (Element == "centered")
	{
		return "kui::UIBox::Align::Centered";
	}
	return "";
}

std::string kui::stringParse::GetBorderType(std::string Element)
{
	if (Element == "none")
	{
		return "kui::UIBox::BorderType::None";
	}
	if (Element == "rounded")
	{
		return "kui::UIBox::BorderType::Rounded";
	}
	if (Element == "border")
	{
		return "kui::UIBox::BorderType::DarkenedEdge";
	}
	return "";
}

bool kui::stringParse::IsNumber(std::string Element)
{
	char* p;
	return(strspn(Element.c_str(), "-.0123456789") == Element.size());
	return !(*p);

}
bool kui::stringParse::IsTranslatedString(std::string Element)
{
	return IsStringToken(Element) && Element[0] == '$';
}

std::string kui::stringParse::ToCppCode(std::string Value)
{
	if (IsNumber(Value))
	{
		return "float(" + Value + ")";
	}
	// (number, number, number) or maybe just (number)
	if (IsVectorToken(Value))
	{
		auto Values = stringParse::SeparateString(Value);

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
			return "GetTranslation(" + Value.substr(1) + ")";
		}
		return Value;
	}

	return Value;
}

kui::stringParse::Size::Size(std::string SizeString, bool Is1D)
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

std::string kui::stringParse::Size::SizeModeToKUISizeMode(std::string Mode)
{
	if (Mode == "pr" || Mode == "px")
	{
		return "kui::UIBox::SizeMode::PixelRelative";
	}
	if (Mode == "ar")
	{
		return "kui::UIBox::SizeMode::AspectRelative";
	}
	return "kui::UIBox::SizeMode::ScreenRelative";
}

std::vector<stringParse::Line> stringParse::SeparateString(std::string String)
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
