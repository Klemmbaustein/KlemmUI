#include <KlemmUI/Markup/MarkupElement.h>
using namespace KlemmUI;

static std::map<std::string, Font*> Fonts;

void MarkupElement::AddFont(std::string FontName, Font* FontPointer)
{
	Fonts.insert({FontName, FontPointer});
}

void KlemmUI::MarkupElement::SetDefaultFont(Font* FontPointer)
{
	AddFont("", FontPointer);
}

Font* MarkupElement::GetFont(std::string FontName)
{
	auto FoundFont = Fonts.find(FontName);

	if (FoundFont != Fonts.end())
	{
		return FoundFont->second;
	}
	return nullptr;
}
