#include <kui/Markup/Markup.h>
#include <kui/Window.h>
#include <map>
#include <kui/UISize.h>
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
	if (Fonts.contains(FontName))
		Fonts.erase(FontName);

	Fonts.insert({ FontName, FontPointer });
}

void kui::MarkupLanguageManager::SetDefaultFont(Font* FontPointer)
{
	AddFont("", FontPointer);
}
AnyContainer MarkupLanguageManager::ListenToGlobal(const char* GlobalName, AnyContainer DefaultValue, void* Target, std::function<void()> OnChanged)
{
	bool Found = false;
	for (auto& i : Globals)
	{
		if (i.first != GlobalName)
		{
			continue;
		}
		Found = true;
		if (!i.second.OnChangedCallbacks.contains(Target))
		{
			i.second.OnChangedCallbacks.insert({ Target, OnChanged });
		}
		return i.second.Value;
	}

	if (!Found)
	{
		Globals.insert({ GlobalName, GlobalInfo(DefaultValue, {{Target, OnChanged}}) });
	}
	return DefaultValue;
}

void MarkupLanguageManager::SetGlobal(const char* GlobalName, AnyContainer Value)
{
	bool Found = false;
	for (auto& i : Globals)
	{
		if (i.first != GlobalName)
		{
			continue;
		}
		Found = true;
		i.second.Value = Value;
		for (auto& Callback : i.second.OnChangedCallbacks)
		{
			Callback.second();
		}
	}

	if (!Found)
	{
		Globals.insert({ GlobalName, GlobalInfo(Value, {}) });
	}
}

void MarkupLanguageManager::RemoveGlobalListener(void* Target)
{
	for (auto& i : Globals)
	{
		auto Found = i.second.OnChangedCallbacks.find(Target);
		if (Found == i.second.OnChangedCallbacks.end())
		{
			continue;
		}

		i.second.OnChangedCallbacks.erase(Found);
	}
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
		Callback.second();
	}
}

Vec2f kui::AnyContainer::AsVec2()
{
	if (Empty)
		return 0;
	try
	{
		return std::any_cast<Vec2f>(Value);
	}
	catch (std::bad_any_cast)
	{
		return std::any_cast<SizeVec>(Value).GetScreen();
	}
}