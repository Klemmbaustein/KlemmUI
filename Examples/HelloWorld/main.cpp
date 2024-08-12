#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <KlemmUI/UI/UITextField.h>
#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/UI/UIButton.h>
#include <iostream>
#include <thread>
#include <filesystem>
using namespace KlemmUI;

void OtherWindow()
{
	return;
	Window win = KlemmUI::Window("woah 2", Window::WindowFlag::Borderless | Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(400, 400));

	win.BorderColor = Vector3f(1, 0, 0);

	Font* fnt = new Font("C:/Windows/Fonts/SegoeUI.ttf");

	UIScrollBox* Scroll = new UIScrollBox(false, 0, true);
	Scroll->SetPosition(-1);
	Scroll->SetMinSize(2);
	Scroll->SetMaxSize(2);

	Scroll->AddChild((new UIText(20, 1, "Hello, world!", fnt))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetPadding(0, 400, 0, 0)
		->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative));

	Scroll->AddChild((new UIButton(true, 0, 1, [&win]() {
		win.Close();
		}))
		->AddChild((new UIText(20, 0, "Close this window", fnt))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)));

	while (win.UpdateWindow())
	{

	}
}

int main()
{
	Application::Initialize("../../Shaders");
	Application::Error::SetErrorCallback([](std::string Message)
		{
			std::cout << "- [kui-Error]: " << Message << std::endl;
		});
	std::thread t = std::thread(OtherWindow);

	Window* MainWindow = new KlemmUI::Window("woah", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(640, 480));

	MainWindow->SetMinSize(Vector2ui(640, 480));
	MainWindow->IsAreaGrabbableCallback = [](KlemmUI::Window* Target) -> bool
		{
			return Target->Input.MousePosition.Y > 0;
		};
	MainWindow->BorderColor = 1;
#if _WIN32
	Font* fnt = new Font("C:/Windows/Fonts/SegoeUI.ttf");
#else
	Font* fnt = new Font("/mnt/c/Windows/Fonts/SegoeUI.ttf");
#endif
	auto line = new UIBackground(0, 1, Vector3f(1, 0, 1), 5);

	(new UIBox(false, -1.0f))
		->SetMinSize(2.0f)
		->SetVerticalAlign(UIBox::Align::Centered)
		->SetHorizontalAlign(UIBox::Align::Centered)
		->AddChild(line
			->SetTryFill(true)
			->SetSizeMode(UIBox::SizeMode::PixelRelative))
		->AddChild((new UIButton(0, 0.2f, 1, nullptr))
			->SetMinSize(0.1f)
			->SetTryFill(true))
		->AddChild((new UIText(20, 1, "Hello, world!", fnt))
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative));

	while (MainWindow->UpdateWindow())
	{
		line->SetColor(MainWindow->Time > 3.0f ? Vector3f(1, 0, 0) : Vector3f(0, 0, 1));
	}

	delete MainWindow;
	t.join();
}

int WinMain()
{
	return main();
}