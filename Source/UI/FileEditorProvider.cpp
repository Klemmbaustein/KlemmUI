#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#include <kui/UI/FileEditorProvider.h>
#include <kui/UI/UITextEditor.h>
#include <fstream>
#include <kui/App.h>
#include <iostream>
#include <kui/Window.h>
#include <stack>
#include <cstring>

kui::FileEditorProvider::FileEditorProvider(std::string Path)
	: FileEditorProvider()
{
	auto Stream = std::ifstream(Path, std::ios::in);
	LoadStream(Stream);
	Stream.close();
}

kui::FileEditorProvider::FileEditorProvider(std::istream& Stream)
	: FileEditorProvider()
{
	LoadStream(Stream);
}

kui::FileEditorProvider::FileEditorProvider()
{
	Keywords = {
	"int",
	"float",
	"bool",
	"obj",
	"array",
	"vec3",
	"vec2",
	"obj",
	"str"
	};
}

void kui::FileEditorProvider::UpdateBracketAreas()
{
	this->BracketAreas.clear();
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

void kui::FileEditorProvider::LoadStream(std::istream& Stream)
{
	// https://en.wikipedia.org/wiki/Byte_order_mark
	static unsigned char UtfBom[] = { 0xef, 0xbb, 0xbf };
	bool IsFirstLine = true;
	std::string NewLine;

	while (!Stream.eof() && !Stream.fail() && !Stream.bad())
	{
		char LineBuffer[4000];

		Stream.getline(LineBuffer, sizeof(LineBuffer));
		LineBuffer[sizeof(LineBuffer) - 1] = 0;

		size_t Start = 0;

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

			Start = IsBom ? 3 : 0;
			IsFirstLine = false;
		}
		NewLine.clear();
		for (size_t i = Start, len = strlen(LineBuffer); i < len; i++)
		{
			if (LineBuffer[i] != '\r')
				NewLine.push_back(LineBuffer[i]);
		}
		this->Lines.push_back(NewLine);
	}

	if (this->Lines.empty())
	{
		this->Lines.push_back({});
	}
	UpdateBracketAreas();
}

void kui::FileEditorProvider::GetPreLine(size_t LineIndex, std::vector<TextSegment>& To)
{
	size_t Length = size_t(std::floor(std::log10(LineIndex + 1))) + 1;
	std::string str;
	str.reserve(8);
	str.resize(6 - Length, ' ');
	str.append(std::to_string(LineIndex + 1));
	str.resize(8, ' ');
	To.push_back(TextSegment(str, LineNumberColor));
}

void kui::FileEditorProvider::GetLine(size_t LineIndex, std::vector<TextSegment>& To)
{
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
	bool IsComment = false;
	bool LastWasComment = false;

	auto ProcessWord = [this, &CurrentWord, &IsString, &Current, &To, &IsComment]() {
		if (!CurrentWord.empty())
		{
			Vec3f NewColor = TextColor;
			if (Keywords.contains(CurrentWord))
			{
				NewColor = KeywordColor;
			}
			else if (IsString)
			{
				NewColor = StringColor;
			}
			else if (IsComment)
			{
				NewColor = CommentColor;
			}
			else
			{
				bool IsDigit = true;
				bool ContainsAnyDigit = false;
				for (auto& i : CurrentWord)
				{
					if (uint8_t(i) < 127 && std::isdigit(i))
					{
						ContainsAnyDigit = true;
					}

					if (uint8_t(i) > 127 || (!std::isdigit(i) && i != '.' && i != '-'))
					{
						if (!ContainsAnyDigit || uint8_t(i) > 127 || !std::isalpha(i))
						{
							IsDigit = false;
						}

					}
				}
				if (IsDigit)
				{
					NewColor = NumberColor;
				}
			}

			if (NewColor != Current.Color)
			{
				To.push_back(Current);
				Current.Text.clear();
				Current.Color = NewColor;
			}
		}
		Current.Text.append(CurrentWord);
		CurrentWord.clear();
		};

	for (char c : Highlighted)
	{
#ifndef WINDOWS
		if (c == '\r')
		{
			continue;
		}
#endif
		if (c == '/' && !IsComment && !IsString)
		{
			if (LastWasComment)
			{
				ProcessWord();
				CurrentWord.push_back('/');
				CurrentWord.push_back('/');
				IsComment = true;
			}
			else
			{
				LastWasComment = true;
			}
			continue;
		}
		if (IsComment)
		{
			CurrentWord.push_back(c);
			continue;
		}
		if (LastWasComment)
		{
			ProcessWord();
			CurrentWord.push_back('/');
			LastWasComment = false;
		}

		if ((IsString && c == '"') || IsComment
			|| (!IsString && (c == ' ' || c == '\t' || SpecialChars.contains(c))))
		{
			if (IsString)
			{
				CurrentWord.push_back(c);
			}

			ProcessWord();

			if (!IsString)
			{
				if (c != ' ' && c != '\t' && TextColor != Current.Color)
				{
					To.push_back(Current);
					Current.Text.clear();
					Current.Color = TextColor;
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
	if (LastWasComment && !IsComment)
	{
		ProcessWord();
		CurrentWord.push_back('/');
		LastWasComment = false;
	}

	ProcessWord();
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
	NextChange.Parts.push_back(ChangePart{
		.Line = Start,
		.Content = Lines[Start],
		.IsRemove = true,
		});

	this->Lines.erase(this->Lines.begin() + Start, this->Lines.begin() + Start + Length);
	UpdateBracketAreas();
	this->ParentEditor->UpdateHighlights = true;
}

void kui::FileEditorProvider::SetLine(size_t Index, const std::vector<TextSegment>& NewLine)
{
	NextChange.Parts.push_back(ChangePart{
		.Line = Index,
		.Content = this->Lines[Index],
		});

	if (!ParentEditor->GetParentWindow()->Input.Text.empty())
	{
		UnDoneChanges = {};
	}
	this->Lines[Index] = TextSegment::CombineToString(NewLine);
	UpdateBracketAreas();
	this->ParentEditor->UpdateHighlights = true;
	std::vector<TextSegment> Segments;
	GetLine(Index, Segments);
	UpdateLine(Index, Segments);
}

void kui::FileEditorProvider::InsertLine(size_t Index, const std::vector<TextSegment>& Content)
{
	NextChange.Parts.push_back(ChangePart{
		.Line = Index,
		.IsAdd = true,
		});

	this->Lines.insert(this->Lines.begin() + Index, TextSegment::CombineToString(Content));

	UpdateBracketAreas();
	this->ParentEditor->UpdateHighlights = true;
	std::vector<TextSegment> Segments;
	GetLine(Index, Segments);
	UpdateLine(Index, Segments);
}

void kui::FileEditorProvider::GetHighlightsForRange(size_t Begin, size_t Length)
{
	for (auto& i : this->BracketAreas)
	{
		this->ParentEditor->HighlightArea(HighlightedArea{
			.Start = i.first,
			.End = i.second,
			.Mode = HighlightMode::HighlightLines,
			.Color = BacketAreaColor,
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
	Out.reserve(this->Lines.size() * 40);
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

void kui::FileEditorProvider::OnLoaded()
{
}

void kui::FileEditorProvider::Update()
{
}

void kui::FileEditorProvider::Undo()
{
	if (Changes.empty())
	{
		return;
	}

	auto& c = Changes.top();

	UnDoneChanges.push(ApplyChange(c));
	Changes.pop();
	Commit();
}

void kui::FileEditorProvider::Redo()
{
	if (UnDoneChanges.empty())
	{
		return;
	}

	auto& c = UnDoneChanges.top();

	Changes.push(ApplyChange(c));
	UnDoneChanges.pop();
	Commit();
}

void kui::FileEditorProvider::Commit()
{
	if (NextChange.Parts.size())
	{
		this->Changes.push(NextChange);
	}

	NextChange = Change();
}

kui::FileEditorProvider::Change kui::FileEditorProvider::ApplyChange(const Change& Target)
{
	Change DoneChanges;

	for (auto p = Target.Parts.rbegin(); p < Target.Parts.rend(); p++)
	{
		if (p->IsAdd)
		{
			DoneChanges.Parts.push_back(ChangePart{
				.Line = p->Line,
				.Content = this->Lines[p->Line],
				.IsRemove = true,
				});
			ParentEditor->RemoveLine(p->Line);
		}
		else if (p->IsRemove)
		{
			DoneChanges.Parts.push_back(ChangePart{
				.Line = p->Line,
				.IsAdd = true,
				});
			std::vector<TextSegment> s = { TextSegment(p->Content, 1) };
			ParentEditor->AddLine(p->Line, s);
		}
		else
		{
			DoneChanges.Parts.push_back(ChangePart{
				.Line = p->Line,
				.Content = this->Lines[p->Line],
				});
			std::vector<TextSegment> s = { TextSegment(p->Content, 1) };
			SetLine(p->Line, s);
			this->Lines[p->Line] = p->Content;
		}
	}

	NextChange = {};

	return DoneChanges;
}

#endif