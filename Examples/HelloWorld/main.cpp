#include <kui/KlemmUI.h>
#include <kui/Platform.h>
#include "HelloWorld.kui.hpp"
#include <thread>
using namespace kui;

int main()
{
	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	Window MainWindow = Window("Hello World", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vec2ui(640, 480));

	MainWindow.SetMinSize(300);

	Font* RobotoFont = new Font("res:Roboto-Regular.ttf");
	MainWindow.Markup.SetDefaultFont(RobotoFont);

	HelloWorld* HelloWorldElement = new HelloWorld();

	while (MainWindow.UpdateWindow())
	{
	}

	delete RobotoFont;
}

int WinMain()
{
	return main();
}
