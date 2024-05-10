#include <KlemmUI/Markup/Markup.h>
#include <KlemmUI/Window.h>
#include <map>
using namespace KlemmUI;

static std::map<std::string, Font*> Fonts;

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
