#pragma once
#include "Markup.h"
#include <string>

namespace kui::markup
{
	class MarkupBox
	{
		bool RegisteredForTranslation = false;
		bool HasGlobal = false;
	protected:
		virtual ~MarkupBox();
		virtual void OnTranslationChanged();
		virtual void OnGlobalChanged();
		std::string GetTranslation(const char* TranslationConstant);
		AnyContainer GetGlobal(const char* GlobalName, AnyContainer DefaultValue);
	};
}