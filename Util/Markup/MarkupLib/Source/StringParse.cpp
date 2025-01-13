#include "Markup/StringParse.h"
#include "Markup/ParseError.h"
#include <unordered_set>
#include <cstring>
#include <sstream>
#include <iostream>
using namespace kui;

static const std::unordered_set<char> Whitespace = {
	' ',
	'\t',
	'\n',
	'\r',
};

static const std::unordered_set<char> SpecialChars = {
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
				OutCode.push_back('\n');
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

stringParse::StringToken stringParse::Line::Previous()
{
	if (StringPos <= Strings.size() && StringPos > 0)
	{
		return Strings[StringPos - 1];
	}
	return GetLast();
}

stringParse::StringToken stringParse::Line::Get()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos++];
	}
	return GetLast();
}

stringParse::StringToken stringParse::Line::Peek()
{
	if (StringPos < Strings.size())
	{
		return Strings[StringPos];
	}
	return GetLast();
}

stringParse::StringToken stringParse::Line::GetUntil(std::string str)
{
	StringToken Condition;
	if (Empty())
	{
		Condition = Previous();
		Condition.Text.clear();
	}
	while (!Empty())
	{
		StringToken Next = Get();
		if (Next.Text == str)
		{
			break;
		}

		Condition = StringToken(Condition.Text + Next.Text, Condition.Empty() ? Next.BeginChar : Condition.BeginChar, Next.EndChar, Next.Line);
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
		StringToken Next = Get();
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
		StringToken Next = Get();

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
			parseError::Error("Expected a matching ']'", Peek());
		}
		else
		{
			parseError::Error("Expected a matching ')'", Peek());
		}
	}
	return Condition;
}

std::vector<stringParse::Line> stringParse::Line::GetLinesInBraces()
{
	uint8_t Depth = 1;
	std::vector<Line> Conditions = {};
	bool Success = false;
	while (!Empty())
	{
		StringToken Next = Get();

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
		parseError::Error("Expected a matching ')'", Peek());
	}
	return Conditions;
}

stringParse::StringToken stringParse::Line::GetLast()
{
	if (Strings.size())
	{
		const StringToken& Last = Strings[Strings.size() - 1];
		return StringToken("", Last.EndChar + 1, Last.EndChar + 1, Last.Line);
	}
	return StringToken("", 0, 0, 0);
}

bool stringParse::Line::Empty() const
{
	return Strings.size() <= StringPos;
}

static void ParseWord(stringParse::StringToken& CurrentWord, stringParse::Line& CurrentLine)
{
	using namespace stringParse;

	if (!CurrentWord.Empty())
	{
		if (CurrentWord.Line == SIZE_MAX)
			CurrentWord.Line = CurrentLine.Index;
		CurrentLine.Strings.push_back(CurrentWord);
	}
	CurrentWord = StringToken();
	CurrentWord.Line = CurrentLine.Index;
}

bool stringParse::IsStringToken(std::string Element)
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

bool kui::stringParse::IsNumber(std::string Element)
{
	if (Element.empty())
		return false;
	return (strspn(Element.c_str(), "-.0123456789") == Element.size());
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
	// (number, number, number) or just (number)
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
		size_t Count = 0;
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
				Count++;
			}
			else
			{
				Value += i;
			}
		}
		if (Count == 3)
			return "kui::Vec3f(" + OutString + ")";
		if (Count == 2)
			return "kui::Vec2f(" + OutString + ")";
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

stringParse::StringToken::StringToken(std::string Text, size_t BeginChar, size_t EndChar, size_t Line)
{
	this->Text = Text;
	this->BeginChar = BeginChar;
	this->EndChar = EndChar;
	this->Line = Line;
}

stringParse::StringToken::StringToken(std::string Text, size_t BeginChar, size_t Line)
	: StringToken(Text, BeginChar, BeginChar + Text.size() - 1, Line)
{
}

void stringParse::StringToken::Add(char c, size_t NewLast)
{
	if (Text.empty() && NewLast != SIZE_MAX)
	{
		BeginChar = NewLast;
	}
	Text.push_back(c);
	if (NewLast == SIZE_MAX)
	{
		EndChar++;
	}
	else
	{
		EndChar = NewLast + 1;
	}
}

bool stringParse::StringToken::IsName() const
{
	if (Text.empty() || !std::isalpha(Text[Text.size() - 1]))
		return false;

	for (char c : Text)
	{
		if (!std::isalnum(c))
		{
			return false;
		}
	}
	return true;
}

kui::stringParse::Size::Size(std::string SizeString, bool Is1D)
{
	// Just a vector/number.
	if ((!Is1D && IsVectorToken(SizeString)) || IsNumber(SizeString))
	{
		SizeValue = SizeString;
		SizeMode.clear();
	}

	bool IsPercent = SizeString[SizeString.size() - 1] == '%';

	if (SizeString.size() <= 2 && !IsPercent)
	{
		// Size suffix is always 2 characters. If there's only 2 characters, there is either no valid size suffix
		// or no number attached to it.
		return;
	}

	std::string SizeSuffix = SizeString.substr(SizeString.size() - 2);
	std::string Value = SizeString.substr(0, SizeString.size() - 2);

	if (IsPercent)
	{
		SizeSuffix = "%";
		try
		{
			Value = std::to_string(std::stof(SizeString.substr(0, SizeString.size() - 1)) / 100.0f);
		}
		catch (std::exception)
		{

		}
	}

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
	// Parent Relative
	if (SizeSuffix == "pn" || SizeSuffix == "%")
	{
		SizeValue = Value;
		SizeMode = "pn";
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
std::string kui::stringParse::Size::ToCppCode(bool IsVector)
{
	if (IsVector)
	{
		return "kui::SizeVec(kui::Vec2f("
			+ stringParse::ToCppCode(SizeValue)
			+ "), "
			+ SizeModeToKUISizeMode(SizeMode)
			+ ")";
	}
	return "kui::UISize(float("
		+ stringParse::ToCppCode(SizeValue)
		+ "), "
		+ SizeModeToKUISizeMode(SizeMode)
		+ ")";
}

std::string kui::stringParse::Size::SizeModeToKUISizeMode(std::string Mode)
{
	if (Mode == "pr" || Mode == "px")
	{
		return "kui::SizeMode::PixelRelative";
	}
	if (Mode == "ar")
	{
		return "kui::SizeMode::AspectRelative";
	}
	if (Mode == "pn")
	{
		return "kui::SizeMode::ParentRelative";
	}
	return "kui::SizeMode::ScreenRelative";
}

std::vector<stringParse::Line> stringParse::SeparateString(std::string String)
{
	String = RemoveComments(String);
	std::vector<Line> Lines;
	Line CurrentLine;
	CurrentLine.Index = 0;
	stringParse::StringToken CurrentWord;
	size_t LineIndex = 0;
	size_t LineCharacter = 0;

	bool InQuotes = false;

	for (char c : String)
	{
		if (c == '\n')
		{
			LineIndex++;
			LineCharacter = 0;
		}
		else
		{
			LineCharacter++;
		}
		CurrentLine.Index = LineIndex;

		if (c == '"')
		{
			if (InQuotes)
			{
				CurrentWord.Add(c, LineCharacter - 1);
				ParseWord(CurrentWord, CurrentLine);
				InQuotes = false;
			}
			else
			{
				ParseWord(CurrentWord, CurrentLine);
				InQuotes = true;
				CurrentWord.Add(c, LineCharacter - 1);
			}
			continue;
		}

		if (InQuotes)
		{
			CurrentWord.Add(c, LineCharacter - 1);
			continue;
		}

		if (c == ';' || c == '}' || c == '{')
		{
			if (c == '{')
			{
				ParseWord(CurrentWord, CurrentLine);
				CurrentWord = StringToken({ c }, LineCharacter - 1, LineIndex);
				ParseWord(CurrentWord, CurrentLine);
			}

			if (!CurrentWord.Empty())
			{
				CurrentLine.Strings.push_back(CurrentWord);
			}
			if (!CurrentLine.Strings.empty())
			{
				Lines.push_back(CurrentLine);
			}

			CurrentLine = Line();
			CurrentWord = StringToken();
			CurrentWord.Line = LineIndex;
			if (c == '}')
			{
				Line l;
				l.Strings = { StringToken({ c }, LineCharacter - 1, LineIndex) };
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
			CurrentWord = StringToken({ c }, LineCharacter - 1, LineIndex);
			ParseWord(CurrentWord, CurrentLine);
		}
		else
		{
			CurrentWord.Add(c, LineCharacter - 1);
		}
	}
	ParseWord(CurrentWord, CurrentLine);

	if (!CurrentLine.Strings.empty())
	{
		Lines.push_back(CurrentLine);
	}
	return Lines;
}

std::optional<stringParse::StringToken> stringParse::GetTokenAt(std::vector<Line> Lines, size_t Character, size_t Line)
{
	for (auto& i : Lines)
	{
		i.ResetPos();

		while (!i.Empty())
		{
			stringParse::StringToken Token = i.Get();
			if (Token.Line == Line && Token.BeginChar <= Character && Token.EndChar >= Character)
			{
				return Token;
			}
		}
	}
	return {};
}
