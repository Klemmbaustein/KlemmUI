#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIButton.h>
#include <KlemmUI/UI/UIBackground.h>
#include <thread>
#include <iostream>

std::thread b;

void OtherWindow()
{
	using namespace KlemmUI;
	
	Window Window2 = Window("KlemmUI 2.0 but THAT'S A 2ND WINDOW HOLY SHIT WHAT???", Window::WindowFlag::Resizable);

	(new UIButton(true, 0, 1, []() {
		}))
		->AddChild((new UIText(10, 0, "AAAAAAAAAAAAAAAAAAAAAAAAAA", new TextRenderer("C:/Windows/Fonts/SegoeUI.ttf")))
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative));


	while (Window2.UpdateWindow())
	{
	}
}

int main()
{
	using namespace KlemmUI;

	Application::Initialize("../../Shaders");

	b = std::thread(OtherWindow);

	{
		Window MainWindow = KlemmUI::Window("KlemmUI 2.0", Window::WindowFlag::Resizable | Window::WindowFlag::Borderless, Window::POSITION_CENTERED, Vector2ui(640, 480));

		MainWindow.IsAreaGrabbableCallback = [](Window* Target) {
			return Target->Input.MousePosition.Y > 0;
			};


		(new UIBox(false, -1.0f))
			->SetMinSize(2.0f)
			->SetVerticalAlign(UIBox::Align::Centered)
			->SetHorizontalAlign(UIBox::Align::Centered)
			->AddChild((new UIBackground(true, 0, 1, Vector2f(0, 1)))
				->SetPadding(6)
				->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative)
				->SetSizeMode(UIBox::SizeMode::PixelRelative)
				->SetTryFill(true))
			->AddChild((new UIText(20, 1, "KlemmUI 2.0 woah", new TextRenderer("C:/Windows/Fonts/SegoeUI.ttf")))
				->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
				->SetPadding(0));

		while (MainWindow.UpdateWindow())
		{
		}
		
	}
	b.join();
}