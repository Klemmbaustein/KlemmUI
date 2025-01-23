#include <kui/KlemmUI.h>
#include <kui/DynamicMarkup.h>
#include <cmath>
using namespace kui;

int main()
{
	app::error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			app::MessageBox(Message, "Error", app::MessageBoxType::Error);
		});

	Window MainWindow = Window("Dynamic markup example", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vec2ui(640, 480));

	Font* Fnt = new Font("res:Roboto-Regular.ttf");
	MainWindow.Markup.SetDefaultFont(Fnt);

	markup::DynamicMarkupContext ctx;
	ctx.LoadFiles({
		markup::MarkupFile{
			.Name = "file1",
			.Content = resource::GetStringResource("dynamic/DynamicMarkup.kui")
		}
		});

	auto Element = new markup::UIDynMarkupBox(&ctx, "DynamicMarkupExample");

	Element->SetVariable("Text", "Hello World!");
	Element->SetVariable("Color", Vec3f(1, 0, 0));

	while (MainWindow.UpdateWindow())
	{
	}

	delete Fnt;
}

int WinMain()
{
	return main();
}
