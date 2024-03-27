#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>
#include <thread>

std::thread b;

void OtherWindow()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));

	KlemmUI::Window Window2 = KlemmUI::Window("KlemmUI 2.0 but THAT'S A 2ND WINDOW HOLY SHIT WHAT???", KlemmUI::Window::WindowFlag::Resizable);

	(new UIText(2, 1, "AAAAAAAAAAAAAAAAAAAAAAAAAA", new TextRenderer("C:/Windows/Fonts/SegoeUI.ttf")))
		->SetTextSizeMode(UIBox::SizeMode::PixelRelative);

	while (Window2.UpdateWindow())
	{
	}
}

int main()
{
	KlemmUI::Application::Initialize("../../Shaders");

	b = std::thread(OtherWindow);

	{
		KlemmUI::Window MainWindow = KlemmUI::Window("KlemmUI 2.0", KlemmUI::Window::WindowFlag::Resizable | KlemmUI::Window::WindowFlag::Borderless, KlemmUI::Window::POSITION_CENTERED, Vector2ui(640, 480));

		MainWindow.IsAreaGrabbableCallback = [](KlemmUI::Window* Target) {
			return Target->Input.MousePosition.Y > 0;
			};


		(new UIBox(false, -1))
			->SetMinSize(2)
			->SetVerticalAlign(UIBox::Align::Centered)
			->SetHorizontalAlign(UIBox::Align::Centered)
			->AddChild((new UIBackground(true, 0, 1, Vector2f(0, 0.005f)))
				->SetTryFill(true))
			->AddChild(new UIText(2, 1, "KlemmUI 2.0 woah", new TextRenderer("C:/Windows/Fonts/SegoeUI.ttf")));

		while (MainWindow.UpdateWindow())
		{
		}
	}
	b.join();
}