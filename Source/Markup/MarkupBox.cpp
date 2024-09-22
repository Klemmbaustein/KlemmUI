#include <kui/Markup/MarkupBox.h>
#include <kui/Window.h>

std::string kui::markup::MarkupBox::GetTranslation(const char* TranslationConstant)
{
	auto& Markup = Window::GetActiveWindow()->Markup;
	if (!RegisteredForTranslation)
	{
		Markup.TranslationChangedCallbacks.push_back([this]() {this->OnTranslationChanged(); });
		RegisteredForTranslation = true;
	}
	return Markup.GetString(TranslationConstant);
}

void kui::markup::MarkupBox::OnTranslationChanged()
{
}
