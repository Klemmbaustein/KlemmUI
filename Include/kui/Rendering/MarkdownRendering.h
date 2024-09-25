#pragma once
#include "../UI/UIText.h"

namespace kui::Markdown
{
	struct MarkdownStyling
	{
		float Width = 1;
		Vec3f Color = 1;
		Font* Text = nullptr;
		float TextSize = 1.0f;
		struct CodeStyling
		{
			Font* CodeText = nullptr;
			Vec3f Color = 1.0f;
			Vec3f BackgroundColor = 0.1f;
			float Rounding = 0;
		};
		CodeStyling Code;
	};

	void RenderMarkdown(std::string Markdown, UIBox* TargetParent, MarkdownStyling Style);
}