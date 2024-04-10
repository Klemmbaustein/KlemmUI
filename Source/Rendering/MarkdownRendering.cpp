#include <KlemmUI/Rendering/MarkdownRendering.h>
#include <KlemmUI/UI/UIBackground.h>
#include <cmath>
#include <KlemmUI/Window.h>
using namespace KlemmUI;

static void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
}

struct MarkdownLine
{
	std::string Text;
	uint8_t HeadingSize = 0;
	size_t Depth = 0;
	bool IsCodeBlock = false;
	std::vector<MarkdownLine> Children;
	bool Seperator = false;
	bool HasHeadingPadding = true;

	void Add(UIBox* TargetParent, const Markdown::MarkdownStyling& Style, float Wrap, bool Small)
	{
		float TextSize = Style.TextSize;

		float HeadingPadding = 0;

		if (HeadingSize)
		{
			TextSize += (5 - HeadingSize) * 0.1f + 0.1f;
			if (HasHeadingPadding)
			{
				HeadingPadding += (5 - HeadingSize) * 0.01f + 0.03f;
			}
		}

		if (Small)
		{
			TextSize *= 0.9f;
		}

		std::string Indent;
		Indent.resize(Depth, ' ');

		Text = Indent + Text;

		auto t = new UIText(TextSize, 1, Text, IsCodeBlock ? Style.Code.CodeText : Style.Text);
		t->Wrap = true;
		t->WrapDistance = Wrap;
		t->SetPadding(0.005f + HeadingPadding, 0.005f, 0.005f, 0.005f);
		TargetParent->AddChild(t);

		if (Seperator)
		{
			TargetParent->AddChild((new UIBackground(true, 0, 1, Vector2f(Style.Width, 0.005f)))
				->SetPadding(0.02f, 0.02f, 0, 0));
		}
	}
};

void Markdown::RenderMarkdown(std::string Markdown, UIBox* TargetParent, MarkdownStyling Style)
{
	ReplaceAll(Markdown, "<br>", "\n");
	ReplaceAll(Markdown, "<br/>", "\n");
	ReplaceAll(Markdown, "*", "");
	ReplaceAll(Markdown, "_", "");
	ReplaceAll(Markdown, "\\<", "<");
	ReplaceAll(Markdown, "\\>", ">");

	ReplaceAll(Markdown, "&amp;", "&");
	ReplaceAll(Markdown, "&nbsp;", " ");

	Markdown.append("\n");

	std::vector<MarkdownLine> Lines;
	std::string CurrentLine;

	size_t LastDepth = 0;
	MarkdownLine* PreviousLine = nullptr;

	bool InCodeBlock = false;

	for (char c : Markdown)
	{
		if (c == '\r')
		{
			continue;
		}
		if (c == '\n')
		{
			if (CurrentLine.empty() && !InCodeBlock)
			{
				continue;
			}
			MarkdownLine NewLine;
			NewLine.Depth = CurrentLine.find_first_not_of(" \t");
			if (NewLine.Depth == std::string::npos)
			{
				if (InCodeBlock)
				{
					PreviousLine->Children.push_back(MarkdownLine());
					CurrentLine.clear();
				}
				continue;
			} 
			else if (NewLine.Depth != std::string::npos && PreviousLine && InCodeBlock)
			{
				NewLine.Depth = std::min(PreviousLine->Depth, NewLine.Depth);
			}

			NewLine.Text = CurrentLine.substr(NewLine.Depth);

			if (NewLine.Text.empty())
			{
				continue;
			}

			bool NewIsCodeBlock = NewLine.Text.substr(0, 3) == "```";
			if (PreviousLine && InCodeBlock)
			{
				if (NewIsCodeBlock)
				{
					InCodeBlock = false;
					CurrentLine.clear();
					continue;
				}
				PreviousLine->Children.push_back(NewLine);
				CurrentLine.clear();
				continue;
			}
			else if (NewIsCodeBlock)
			{
				NewLine.Text = NewLine.Text.substr(3);
				NewLine.IsCodeBlock = true;
				InCodeBlock = true;
				Lines.push_back(NewLine);
				PreviousLine = &Lines[Lines.size() - 1];
			}
			else
			{
				if (CurrentLine.find_first_not_of("=") == std::string::npos && PreviousLine)
				{
					PreviousLine->HeadingSize = 1;
					PreviousLine->Seperator = true;
					CurrentLine.clear();
					continue;
				}
				NewLine.HeadingSize = NewLine.Text.find_first_not_of("#");
				if (NewLine.HeadingSize > 5)
				{
					NewLine.HeadingSize = 0;
				}
				if (NewLine.HeadingSize)
				{
					NewLine.Text = NewLine.Text.substr(NewLine.HeadingSize);

					if (NewLine.Text[0] == ' ')
					{
						NewLine.Text = NewLine.Text.substr(1);
					}

					NewLine.Seperator = NewLine.HeadingSize == 1;
				}
				if (NewLine.Text == "<hr>")
				{
					NewLine.Text.clear();
					NewLine.Seperator = true;
				}
				Lines.push_back(NewLine);
				PreviousLine = &Lines[Lines.size() - 1];
			}
			CurrentLine.clear();
			continue;
		}
		CurrentLine.append({ c });
	}
	
	TargetParent->SetHorizontal(false);
	TargetParent->SetVerticalAlign(UIBox::Align::Reverse);

	for (size_t i = 0; i < Lines.size(); i++)
	{
		auto& ln = Lines[i];
		size_t WrapLength = (size_t)(Style.Width * 90.0f * Style.TextSize * Window::GetActiveWindow()->GetAspectRatio());

		if (ln.HeadingSize)
		{
			WrapLength = (size_t)((float)WrapLength / ((float)(5 - ln.HeadingSize) / 2.2f));
		}
		
		if (ln.IsCodeBlock)
		{
			continue;
		}
		else if (ln.Text.find("`") == std::string::npos)
		{
			continue;
		}
		size_t WrapSize = 0;
		size_t it = 0;
		size_t LastSpace = 0;
		bool InCodeSegment = false;
		char Last = 0;
		for (char c : ln.Text)
		{
			if (c == '`')
			{
				InCodeSegment = !InCodeSegment;
				LastSpace = it;
			}

			if (c == ' ')
			{
				LastSpace = it;
			}

			WrapSize += InCodeSegment ? 2 : 1;

			it++;
			if (WrapSize >= WrapLength)
			{
				if (LastSpace == 0)
				{
					LastSpace = it - 1;
				}

				MarkdownLine Next = ln;
				Next.Text = ln.Text.substr(LastSpace + 1);
				if (InCodeSegment)
				{
					Next.Text = "`" + Next.Text;
				}
				Next.HasHeadingPadding = false;
				ln.Text = ln.Text.substr(0, LastSpace + 1);
				Lines.insert(Lines.begin() + i + 1, Next);
				break;
			}
			Last = c;
		}
	}
	for (size_t i = 0; i < Lines.size(); i++)
	{
		auto& ln = Lines[i];
		if (ln.IsCodeBlock)
		{
			continue;
		}
		else if (ln.Text.find("`") == std::string::npos)
		{
			continue;
		}
		std::vector<MarkdownLine> New;

		MarkdownLine NewSegment;
		NewSegment.HasHeadingPadding = ln.HasHeadingPadding;
		NewSegment.HeadingSize = ln.HeadingSize;
		bool InCodeSegment = false;
		for (char c : ln.Text)
		{
			if (c == '`')
			{
				New.push_back(NewSegment);
				NewSegment = MarkdownLine();
				NewSegment.HasHeadingPadding = ln.HasHeadingPadding;
				NewSegment.HeadingSize = ln.HeadingSize;
				if (!InCodeSegment)
				{
					NewSegment.IsCodeBlock = true;
					InCodeSegment = true;
				}
				else
				{
					InCodeSegment = false;
				}
				continue;
			}
			NewSegment.Text.push_back(c);

		}
		New.push_back(NewSegment);

		ln = MarkdownLine();
		ln.Children = New;
	}

	for (auto& i : Lines)
	{
		bool LineSeperator = i.Text.find_first_not_of("-") == std::string::npos && !i.Text.empty();
		if ((i.Text.empty() && !i.IsCodeBlock && !i.Children.size()) || LineSeperator)
		{
			if (i.Seperator || LineSeperator)
			{
				TargetParent->AddChild((new UIBackground(true, 0, 1, Vector2f(Style.Width, 0.005f)))
					->SetPadding(0.02f, 0.02f, 0, 0));
			}
			continue;
		}
		if (i.Children.size() && i.IsCodeBlock)
		{
			UIBackground* Title = new UIBackground(true, 0, 0.1f, Vector2f(0));
			UIBackground* CodeBackground = new UIBackground(false, 0, Style.Code.BackgroundColor);
			CodeBackground->AddChild(Title
				->SetPadding(0)
				->AddChild(new UIText(Style.TextSize, Style.Code.Color, i.Text, Style.Text)));

			size_t LineNumber = 0;
			for (auto& ln : i.Children)
			{
				std::string LineNumberString = std::to_string(++LineNumber);
				LineNumberString.resize(5, ' ');
				CodeBackground->AddChild((new UIText(Style.TextSize * 0.9f,
					{ 
						TextSegment(LineNumberString, Style.Code.Color * 0.5f),
						TextSegment(ln.Text, Style.Code.Color) 
					}, Style.Code.CodeText))
					->SetWrapEnabled(true, Style.Width * 0.7f, UIBox::SizeMode::ScreenRelative)
					->SetPadding(0.005f));
			}
			Title->SetMinSize(Vector2f(Style.Width, 0));
			TargetParent->AddChild(CodeBackground
				->SetPadding(0.02f, 0.02f, 0, 0)
				->SetBorder(UIBox::BorderType::Rounded, Style.Code.Rounding));
			continue;
		}
		if (i.Children.size())
		{
			UIBox* Sidebox = new UIBox(true, 0);
			Sidebox->SetPadding(0);
			for (auto& c : i.Children)
			{
				if (c.Text.empty())
				{
					continue;
				}
				UIBox* Target = Sidebox;
				if (c.IsCodeBlock)
				{
					Target = (new UIBackground(false, 0, Style.Code.BackgroundColor))
						->SetBorder(UIBox::BorderType::Rounded, Style.Code.Rounding)
						->SetPadding(0);
					Sidebox->AddChild(Target);
				}
				c.Add(Target, Style, 999, c.IsCodeBlock);
			}
			TargetParent->AddChild(Sidebox);
		}
		else
		{
			float TextSize = Style.TextSize;
			if (i.HeadingSize)
			{
				TextSize += (5 - i.HeadingSize) * 0.05f + 0.1f;
			}
			i.Add(TargetParent, Style, Style.Width * 0.4f * (1 / TextSize), false);
		}
	}
}
