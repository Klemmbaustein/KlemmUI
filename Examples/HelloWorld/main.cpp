#include <kui/KlemmUI.h>
#include "elements/HelloWorld.hpp"
using namespace kui;

int main()
{
	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	Window MainWindow = Window("Hello, World!", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Window::SIZE_DEFAULT);

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
