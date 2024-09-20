#include <KlemmUI/Markup/Markup.h>
#include <KlemmUI/Window.h>
#include <map>
using namespace KlemmUI;

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

void KlemmUI::MarkupLanguageManager::SetDefaultFont(Font* FontPointer)
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

MarkupLanguageManager* KlemmUI::MarkupLanguageManager::GetActive()
{
	return &Window::GetActiveWindow()->Markup;
}

void KlemmUI::MarkupLanguageManager::OnTranslationChanged()
{
	for (const auto& Callback : TranslationChangedCallbacks)
	{
		Callback();
	}
}
