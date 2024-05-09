#pragma once
#include "../UI/UIBox.h"
#include "../Rendering/Text/Font.h"

namespace KlemmUI
{
	class MarkupElement
	{
	public:
		static void AddFont(std::string FontName, Font* FontPointer);
		static void SetDefaultFont(Font* FontPointer);
		static Font* GetFont(std::string FontName);
	};
}