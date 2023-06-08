#pragma once
#include <UI/UIText.h>

namespace Markdown
{
	void RenderMarkdown(std::string Markdown, float Width, UIBox* TargetParent, Vector3f32 Color, TextRenderer* Font);
}