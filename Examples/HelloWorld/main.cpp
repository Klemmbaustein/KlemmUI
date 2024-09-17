#include <KlemmUI/Window.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <KlemmUI/UI/UITextField.h>
#include <KlemmUI/Application.h>
#include <thread>
using namespace KlemmUI;

void OtherWindow()
{
	Window* MainWindow = new Window("Other test window", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(640, 480));
	while (MainWindow->UpdateWindow())
	{
	}
	delete MainWindow;
}

int main()
{
	Application::Error::SetErrorCallback([](std::string Message, bool Fatal)
		{
			KlemmUI::Application::MessageBox(Message, "Error", Application::MessageBoxType::Error);
		});
	Window MainWindow = Window("Hello, World!", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(800, 600));

	MainWindow.SetMinSize(Vector2ui(800, 600));

	MainWindow.IsAreaGrabbableCallback = [](KlemmUI::Window* Target) -> bool
		{
			return true;
		};

	Font* Fnt = new Font("res:Roboto-Regular.ttf");

	UIText* Text = new UIText(9, 1, "Hello, World!", Fnt);

	(new UIBackground(true, -0.95f, Vector3f(0.25f), 1.9f))
		->SetCorner(5, UIBox::SizeMode::PixelRelative)
		->SetBorder(1, UIBox::SizeMode::PixelRelative)
		->SetCorners(false, false, true, true)
		->SetBorderColor(Vector3f(1, 0.5f, 0))
		->AddChild(Text
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
			->SetPadding(5)
			->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative));

	std::thread Other = std::thread(OtherWindow);
	Other.detach();

	(new UITextField(Vector2f(-0.25f, 0), 0, Fnt, []() { }))
		->SetHintText("Text input test")
		->SetTextSize(12)
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
		->SetMinSize(Vector2f(0.5f, 0));

	while (MainWindow.UpdateWindow())
	{
	}

	delete Fnt;
}

int WinMain()
{
	return main();
}
