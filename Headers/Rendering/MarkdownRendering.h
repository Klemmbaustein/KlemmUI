#pragma once
#include "../UI/UIText.h"

namespace Markdown
{
	struct MarkdownStyling
	{
		float Width = 1;
		Vector3f32 Color = 1;
		TextRenderer* Text = nullptr;
		float TextSize = 0.3f;
		struct CodeStyling
		{
			TextRenderer* CodeText = nullptr;
			Vector3f32 Color = 1;
			Vector3f32 BackgroundColor = 0.1;
			float Rounding = 0;
		};
		CodeStyling Code;
	};

	void RenderMarkdown(std::string Markdown, UIBox* TargetParent, MarkdownStyling Style);
}