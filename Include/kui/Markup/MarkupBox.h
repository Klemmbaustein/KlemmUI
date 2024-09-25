#pragma once
#include <string>

namespace kui::markup
{
	class MarkupBox
	{
		bool RegisteredForTranslation = false;
	protected:
		virtual void OnTranslationChanged();
		std::string GetTranslation(const char* TranslationConstant);
	};
}