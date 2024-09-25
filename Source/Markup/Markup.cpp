#include <kui/Markup/Markup.h>
#include <kui/Window.h>
#include <map>
using namespace kui;

void MarkupLanguageManager::SetGetStringFunction(std::function<std::string(std::string)> NewFunction)
{
	GetStringFunction = NewFunction;
}

std::string MarkupLanguageManager::GetString(std::string From)
{
	if (!GetStringFunction)
	{
		return "";
	}
	return GetStringFunction(From);
}

void MarkupLanguageManager::AddFont(std::string FontName, Font* FontPointer)
{
	Fonts.insert({FontName, FontPointer});
}

void kui::MarkupLanguageManager::SetDefaultFont(Font* FontPointer)
{
	AddFont("", FontPointer);
}

Font* MarkupLanguageManager::GetFont(std::string FontName)
{
	auto FoundFont = Fonts.find(FontName);

	if (FoundFont != Fonts.end())
	{
		return FoundFont->second;
	}
	return nullptr;
}

MarkupLanguageManager* kui::MarkupLanguageManager::GetActive()
{
	return &Window::GetActiveWindow()->Markup;
}

void kui::MarkupLanguageManager::OnTranslationChanged()
{
	for (const auto& Callback : TranslationChangedCallbacks)
	{
		Callback();
	}
}
