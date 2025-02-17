#include <kui/KlemmUI.h>
#include <kui/Platform.h>
#include "HelloWorld.kui.hpp"
#include <thread>
using namespace kui;

void w2()
{
	Window MainWindow = Window("2!", Window::WindowFlag::Resizable);
	MainWindow.IsAreaGrabbableCallback = [](Window* w)
		{
			return w->Input.MousePosition.Y > 0.5;
		};

	auto Text = new UITextField(0, 0, new Font("res:Roboto-Regular.ttf"), nullptr);

	Text->SetPosition(-0.5f);
	Text->SetMinSize(1);
	Text->SetBorder(1_px, 1);
	Text->SetTextSize(13_px);

	while (MainWindow.UpdateWindow())
	{

	}
}

int main()
{
	platform::linux::AlwaysUseWayland();

	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	std::thread t{ w2 };
	t.detach();

	Window MainWindow = Window(app::SelectFileDialog(true), Window::WindowFlag::Resizable | Window::WindowFlag::Borderless, Window::POSITION_CENTERED, Vec2ui(640, 480));
	MainWindow.IsAreaGrabbableCallback = [](Window* w)
		{
			return w->Input.MousePosition.Y > 0.5;
		};

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
