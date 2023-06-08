#pragma once
#include <UI/UIText.h>

namespace Markdown
{
	void RenderMarkdown(std::string Markdown, UIBox* TargetParent, Vector3f32 Color, TextRenderer* Font);
}