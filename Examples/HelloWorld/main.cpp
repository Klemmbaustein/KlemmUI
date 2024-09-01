#include <KlemmUI/Window.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Image.h>
#include <KlemmUI/Timer.h>

int main()
{
	using namespace KlemmUI;

	Application::Error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			KlemmUI::Application::MessageBox(Message, "Error", Application::MessageBoxType::Error);
		});

	Window MainWindow = Window("Hello, World!", Window::WindowFlag::Resizable, Window::POSITION_CENTERED);

	MainWindow.IsAreaGrabbableCallback = [](KlemmUI::Window* Target) -> bool
		{
			return true;
		};

	(new UIBackground(true, -0.95f, Vector3f(0.25f), 1.9f))
		->SetCorner(5, UIBox::SizeMode::PixelRelative)
		->SetBorder(1, UIBox::SizeMode::PixelRelative)
		->SetCorners(false, false, true, true)
		->SetBorderColor(Vector3f(1, 0.5f, 0))
		->AddChild((new UIText(9, 1, "Hello, World!", new Font("file:C:/Windows/Fonts/segoeui.ttf")))
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
			->SetPadding(5)
			->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative));

	Timer LoadingTimer;

	while (MainWindow.UpdateWindow())
	{
	}
}

int WinMain()
{
	return main();
}
