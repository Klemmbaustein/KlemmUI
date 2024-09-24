#include <kui/KlemmUI.h>
#include <map>
#include "elements/TranslationExample.hpp"
using namespace kui;

static const char* GermanTitle = "Übersetzungsbeispiel";
static const char* EnglishTitle = "Translation Example";

int main()
{
	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	Window MainWindow = Window(EnglishTitle, Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vec2ui(800, 600));

	bool IsGerman = false;

	Font* Fnt = new Font("res:Roboto-Regular.ttf");
	MainWindow.Markup.SetDefaultFont(Fnt);

	// The GetStringFunction is used to get translations.
	MainWindow.Markup.SetGetStringFunction([&IsGerman](std::string str) -> std::string
		{
			// You'd probably want to read translations from some kind of
			// resource (json) file instead of hard coding them here.
			static std::map<std::string, std::string> TranslationsEnglish
			{
				{"hello_world", "Hello World!"},
				{"switch_language", "Switch language"},
			};

			static std::map<std::string, std::string> TranslationsGerman
			{
				{"hello_world", "Hallo Welt!"},
				{"switch_language", "Sprache wechseln"},
			};

			if (IsGerman && TranslationsGerman.contains(str))
			{
				return TranslationsGerman[str];
			}
			else if (TranslationsEnglish.contains(str))
			{
				return TranslationsEnglish[str];
			}
			return str;
		});

	TranslationExample* HelloWorldElement = new TranslationExample();

	HelloWorldElement->switchLanguageButton->OnClicked = [&IsGerman, &MainWindow]
		{
			// When the button on the element is clicked, switch the language.
			IsGerman = !IsGerman;
			MainWindow.SetTitle(IsGerman ? GermanTitle : EnglishTitle);
			// This updates all elements using a translation string (for example: text = $"some_translated_text")
			MainWindow.Markup.OnTranslationChanged();
		};

	while (MainWindow.UpdateWindow())
	{
	}

	delete Fnt;
}

int WinMain()
{
	return main();
}
