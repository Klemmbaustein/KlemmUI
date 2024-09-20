#include <kui/KlemmUI.h>
#include "elements/HelloWorld.hpp"
using namespace kui;


int main()
{
	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	Window MainWindow = Window("Hello, World!", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vec2ui(800, 600));

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
