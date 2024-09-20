#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include "elements/HelloWorld.hpp"
using namespace KlemmUI;


int main()
{
	Application::Error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			KlemmUI::Application::MessageBox(Message, "Error", Application::MessageBoxType::Error);
		});

	Window MainWindow = Window("Hello, World!", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(800, 600));

	Font* Fnt = new Font("res:Roboto-Regular.ttf");
	MainWindow.Markup.SetDefaultFont(Fnt);

	HelloWorld* HelloWorldElement = new HelloWorld();

	while (MainWindow.UpdateWindow())
	{
	}
	
	delete Fnt;
}

int WinMain()
{
	return main();
}
