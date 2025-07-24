#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#include <kui/UI/FileEditorProvider.h>
#include <kui/UI/UITextEditor.h>
#include <fstream>
#include <kui/App.h>
#include <iostream>
#include <stack>

kui::FileEditorProvider::FileEditorProvider(std::string Path)
{
	std::ifstream Stream = std::ifstream(Path, std::ios::in);

	// https://en.wikipedia.org/wiki/Byte_order_mark
	static unsigned char UtfBom[] = { 0xef, 0xbb, 0xbf };
	bool IsFirstLine = true;

	while (!Stream.eof())
	{
		char LineBuffer[4000];

		Stream.getline(LineBuffer, sizeof(LineBuffer));
		LineBuffer[sizeof(LineBuffer) - 1] = 0;

		if (IsFirstLine)
		{
			bool IsBom = true;

			for (size_t i = 0; i < 3; i++)
			{
				if (UtfBom[i] != (unsigned char)(LineBuffer[i]))
				{
					IsBom = false;
					break;
				}
			}

			if (IsBom)
			{
				this->Lines.push_back(LineBuffer + 3);
			}
			else
			{
				this->Lines.push_back(LineBuffer);
			}
			IsFirstLine = false;
		}
		else
		{
			this->Lines.push_back(LineBuffer);
		}
	}

	std::stack<std::pair<EditorPosition, EditorPosition>> FoundBracketAreas;

	size_t Index = 0;
	for (auto& i : this->Lines)
	{
		auto Open = i.find_first_of("[{");
		auto Close = i.find_first_of("]}");
		if (Open != std::string::npos && Close != std::string::npos)
		{
			Index++;
			continue;
		}
		if (Open != std::string::npos)
		{
			FoundBracketAreas.push({ EditorPosition(Open, Index) , EditorPosition() });
		}
		if (Close != std::string::npos)
		{
			if (!FoundBracketAreas.empty())
			{

				auto& Top = FoundBracketAreas.top();
				Top.second = EditorPosition(Close, Index);
				if (Top.first != Top.second)
				{
					this->BracketAreas.push_back(Top);
				}
				FoundBracketAreas.pop();
			}
		}

		Index++;
	}
}

void kui::FileEditorProvider::GetPreLine(size_t LineIndex, std::vector<TextSegment>& To)
{
	size_t Length = size_t(std::floor(std::log10(LineIndex + 1))) + 1;
	std::string str;
	str.reserve(8);
	str.resize(6 - Length, ' ');
	str.append(std::to_string(LineIndex + 1));
	str.resize(8, ' ');
	To.push_back(TextSegment(str, 0.75f));
}

void kui::FileEditorProvider::GetLine(size_t LineIndex, std::vector<TextSegment>& To)
{
	static std::set<std::string> Keywords = {
		"int",
		"float",
		"bool",
		"double",
		"long",
		"vec3",
		"vec2",
		"obj",
		"array",
		"string",
		"byte",
		"await",
		"if",
		"else",
		"async",
		"foreach",
		"for",
		"while",
		"is",
		"null",
		"true",
		"false",
		"new",
		"throw",
		"break",
		"continue",
		"switch",
		"using",
		"namespace",
		"get",
		"set",
		"_",
		"public",
		"private",
		"protected",
		"var",
		"class",
		"struct",
		"case",
		"return",
		"as",
		"is",
		"in",
		"const",
		"readonly",
		"static"
	};

	static std::set<char> SpecialChars = {
		'<', '>',
		'[', ']',
		'{', '}',
		'(', ')',
		'=', '^',
		'+', '-',
		'*', '/',
		'$', '%',
		'?', '!',
		';', ',',
		':',
	};

	const std::string& Highlighted = this->Lines[LineIndex];

	if (Highlighted.empty())
	{
		return;
	}

	TextSegment Current = TextSegment("", 1);
	std::string CurrentWord;
	bool IsString = false;

	auto ProcessWord = [&CurrentWord, &IsString, &Current, &To]()
		{
			if (!CurrentWord.empty())
			{
				Vec3f NewColor = 1;
				if (Keywords.contains(CurrentWord))
				{
					NewColor = Vec3f(1.0f, 0.2f, 0.5f);
				}
				else if (IsString)
				{
					NewColor = Vec3f(0.5f, 1.0f, 0.2f);
				}
				else
				{
					bool IsDigit = true;
					for (auto& i : CurrentWord)
					{
						if (!std::isdigit(i) && i != '.' && i != '-')
						{
							IsDigit = false;
						}
					}
					if (IsDigit)
					{
						NewColor = Vec3f(0.5f, 0.7f, 1.0f);
					}
				}

				if (NewColor != Current.Color)
				{
					To.push_back(Current);
					Current.Text.clear();
					Current.Color = NewColor;
				}
			}
		};

	for (char c : Highlighted)
	{
		if ((IsString && c == '"')
			|| (!IsString && (c == ' ' || c == '\t' || SpecialChars.contains(c))))
		{
			if (IsString)
			{
				CurrentWord.push_back(c);
			}

			ProcessWord();
			
			Current.Text.append(CurrentWord);
			CurrentWord.clear();
			if (!IsString)
			{
				if (c != ' ' && c != '\t' && Vec3f(1) != Current.Color)
				{
					To.push_back(Current);
					Current.Text.clear();
					Current.Color = 1;
				}
				Current.Text.push_back(c);
			}
			IsString = false;
		}
		else
		{
			if (CurrentWord.empty() && c == '"')
			{
				IsString = true;
			}
			CurrentWord.push_back(c);
		}
	}

	ProcessWord();
	Current.Text.append(CurrentWord);
	To.push_back(Current);
}

size_t kui::FileEditorProvider::GetLineCount()
{
	return this->Lines.size();
}

size_t kui::FileEditorProvider::GetPreLineSize()
{
	return 8;
}

void kui::FileEditorProvider::RemoveLines(size_t Start, size_t Length)
{
	this->Lines.erase(this->Lines.begin() + Start, this->Lines.begin() + Start + Length);
}

void kui::FileEditorProvider::SetLine(size_t Index, const std::vector<TextSegment>& NewLine)
{
	this->Lines[Index] = TextSegment::CombineToString(NewLine);

	//std::vector<TextSegment> Segments;
	//GetLine(Index, Segments);
	//UpdateLine(Index, Segments);
}

void kui::FileEditorProvider::InsertLine(size_t Index, const std::vector<TextSegment>& Content)
{
	this->Lines.insert(this->Lines.begin() + Index, TextSegment::CombineToString(Content));

	//std::vector<TextSegment> Segments;
	//GetLine(Index, Segments);
	//UpdateLine(Index, Segments);
}

void kui::FileEditorProvider::GetHighlightsForRange(size_t Begin, size_t Length)
{
	for (auto& i : this->BracketAreas)
	{
		this->ParentEditor->HighlightArea(HighlightedArea{
			.Start = i.first,
			.End = i.second,
			.Mode = HighlightMode::HighlightLines,
			.Color = 0.5f,
			});
	}
}

void kui::FileEditorProvider::DumpContent()
{
	for (auto& i : this->Lines)
	{
		std::cerr << i << std::endl;
	}
}
std::string kui::FileEditorProvider::GetContent()
{
	std::string Out;
	for (auto& i : this->Lines)
	{
		Out.append(i);
		Out.push_back('\n');
	}
	if (!Out.empty())
	{
		Out.pop_back();
	}
	return Out;
}
#endif