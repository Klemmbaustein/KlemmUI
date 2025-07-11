#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#pragma once
#include "TextEditor.h"
#include <kui/Vec2.h>
#include <kui/Vec3.h>
#include <kui/UI/UIManager.h>

namespace kui
{
	class UITextEditor;

	struct HighlightSegment
	{
		Vec2f Position;
		Vec2f Size;
	};

	enum class HighlightMode
	{
		HighlightText,
		HighlightLines,
	};

	struct HighlightedArea
	{
		EditorPosition Start;
		EditorPosition End;
		HighlightMode Mode = HighlightMode::HighlightText;
		std::vector<HighlightSegment> Segments;
		Vec3f Color = 1;
		int32_t Priority = 1;

		void GenerateSegments(UITextEditor* Editor);
	};
}
#endif