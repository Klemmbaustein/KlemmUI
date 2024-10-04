#pragma once
#include <string>

namespace kui::markup
{
	class MarkupBox
	{
		bool RegisteredForTranslation = false;
	protected:
		virtual ~MarkupBox();
		virtual void OnTranslationChanged();
		std::string GetTranslation(const char* TranslationConstant);
	};
}