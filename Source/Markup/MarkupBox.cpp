#include <kui/Markup/MarkupBox.h>
#include <kui/Window.h>

std::string kui::markup::MarkupBox::GetTranslation(const char* TranslationConstant)
{
	auto& Markup = Window::GetActiveWindow()->Markup;
	if (!RegisteredForTranslation)
	{
		Markup.TranslationChangedCallbacks.push_back({this, [this]() {this->OnTranslationChanged(); } });
		RegisteredForTranslation = true;
	}
	return Markup.GetString(TranslationConstant);
}

kui::AnyContainer kui::markup::MarkupBox::GetGlobal(const char* GlobalName, AnyContainer DefaultValue)
{
	auto& Markup = Window::GetActiveWindow()->Markup;
	HasGlobal = true;
	return Markup.ListenToGlobal(GlobalName, DefaultValue, this, [this]() {this->OnGlobalChanged(); });
}

void kui::markup::MarkupBox::OnTranslationChanged()
{
}

void kui::markup::MarkupBox::OnGlobalChanged()
{
}

kui::markup::MarkupBox::~MarkupBox()
{
	if (RegisteredForTranslation && Window::GetActiveWindow())
	{
		auto& Callbacks = Window::GetActiveWindow()->Markup.TranslationChangedCallbacks;
		for (size_t i = 0; i < Callbacks.size(); i++)
		{
			if (Callbacks[i].first == this)
			{
				Callbacks.erase(Callbacks.begin() + i);
				break;
			}
		}
	}

	if (HasGlobal && Window::GetActiveWindow())
	{
		auto& Markup = Window::GetActiveWindow()->Markup;
		Markup.RemoveGlobalListener(this);
	}
}