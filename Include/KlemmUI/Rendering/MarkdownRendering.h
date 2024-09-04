#pragma once
#include "../UI/UIText.h"

namespace KlemmUI::Markdown
{
	struct MarkdownStyling
	{
		float Width = 1;
		Vector3f Color = 1;
		Font* Text = nullptr;
		float TextSize = 1.0f;
		struct CodeStyling
		{
			Font* CodeText = nullptr;
			Vector3f Color = 1.0f;
			Vector3f BackgroundColor = 0.1f;
			float Rounding = 0;
		};
		CodeStyling Code;
	};

	void RenderMarkdown(std::string Markdown, UIBox* TargetParent, MarkdownStyling Style);
}