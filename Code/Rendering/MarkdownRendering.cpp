#include <Rendering/MarkdownRendering.h>
#include <regex>
#include <UI/UIBackground.h>

void Markdown::RenderMarkdown(std::string Markdown, UIBox* TargetParent, Vector3f32 Color, TextRenderer* Font)
{
	// Backslash.
	Markdown = std::regex_replace(Markdown, std::regex("<br>"), "\n");
	Markdown = std::regex_replace(Markdown, std::regex("<br/>"), "\n");
	Markdown = std::regex_replace(Markdown, std::regex("\\\\r\\\\n"), "\n");
	Markdown = std::regex_replace(Markdown, std::regex("\\\\n"), "\n");

	// No.
	Markdown = std::regex_replace(Markdown, std::regex("\\*"), "");
	Markdown = std::regex_replace(Markdown, std::regex("\\_"), "");

	Markdown = std::regex_replace(Markdown, std::regex("\\&amp;"), "&");
	Markdown = std::regex_replace(Markdown, std::regex("\\&nbsp;"), " ");

	Markdown.append("\n");

	std::vector<std::string> Lines;
	std::string CurrentLine;

	for (char c : Markdown)
	{
		if (c == '\n')
		{
			Lines.push_back(CurrentLine);
			CurrentLine.clear();
			continue;
		}
		CurrentLine.append({ c });
	}

	for (auto& i : Lines)
	{
		if (i.empty())
			continue;

		float size = 0.3;
		if (i.substr(0, 5) == "#####")
		{
			size = 0.4;
			i = i.substr(3);
		}
		else if (i.substr(0, 4) == "####")
		{
			size = 0.4;
			i = i.substr(3);
		}
		else if (i.substr(0, 3) == "###")
		{
			size = 0.4;
			i = i.substr(3);
		}
		else if (i.substr(0, 2) == "##")
		{
			size = 0.5;
			i = i.substr(2);
		}
		else if (i.substr(0, 1) == "#")
		{
			size = 0.6;
			i = i.substr(1);
		}

		std::string SeperatorRemovedString = std::regex_replace(i, std::regex("<hr>"), "");

		auto DescriptionText = new UIText(size, 1, SeperatorRemovedString, Font);
		DescriptionText->Wrap = true;
		DescriptionText->WrapDistance = 0.9 * (1 / size);
		DescriptionText->SetPadding(size >= 0.4f ? 0.01 : 0, 0, size >= 0.4f ? 0 : 0.02, 0);
		TargetParent->AddChild(DescriptionText);


		if (size >= 0.6f || i != SeperatorRemovedString)
		{
			TargetParent->AddChild((new UIBackground(true, 0, 1, Vector2f(1.15, 0.005)))
				->SetPadding(0.0, 0.02, 0, 0));
		}
	}
}
