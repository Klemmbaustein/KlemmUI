#include <KlemmUI/Window.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UIBackground.h>

int main()
{
	using namespace KlemmUI;

	Application::Initialize("../../Shaders");

	Window MainWindow = KlemmUI::Window("KlemmUI 2", Window::WindowFlag::Resizable, Window::POSITION_CENTERED, Vector2ui(640, 480));

	MainWindow.SetMinSize(Vector2ui(640, 480));

	(new UIBox(false, -1.0f))
		->SetMinSize(2.0f)
		->SetVerticalAlign(UIBox::Align::Centered)
		->SetHorizontalAlign(UIBox::Align::Centered)
		->AddChild((new UIBackground(true, 0, 1, Vector2f(0, 1)))
			->SetPadding(6)
			->SetPaddingSizeMode(UIBox::SizeMode::PixelRelative)
			->SetSizeMode(UIBox::SizeMode::PixelRelative)
			->SetTryFill(true))
		->AddChild((new UIText(20, 1, "Hello, world!", new Font("C:/Windows/Fonts/SegoeUI.ttf")))
			->SetTextSizeMode(UIBox::SizeMode::PixelRelative)
			->SetPadding(0));

	while (MainWindow.UpdateWindow())
	{
	}
}