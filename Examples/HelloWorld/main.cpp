#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <thread>

std::thread b;

void OtherWindow()
{
	KlemmUI::Window Window2 = KlemmUI::Window("KlemmUI 2 but THAT'S A 2ND WINDOW HOLY SHIT WHAT???", KlemmUI::Window::POSITION_CENTERED, 800);

	new UIText(2, 1, "AAAAAAAAAAAAAAAAAAAAAAAAAA", new TextRenderer("C:/Windows/Fonts/segoeui.ttf"));

	while (Window2.UpdateWindow())
	{
	}
}

int main()
{
	KlemmUI::Application::SetShaderPath("../../Shaders");
	KlemmUI::Window MainWindow = KlemmUI::Window("KlemmUI 2", KlemmUI::Window::POSITION_CENTERED, Vector2ui(640, 480));

	new UIText(1.8f, 1, "test", new TextRenderer("C:/Windows/Fonts/segoeui.ttf"));

	b = std::thread(OtherWindow);


	while (MainWindow.UpdateWindow())
	{
	}
	b.detach();
}