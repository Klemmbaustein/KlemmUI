#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>

int main()
{
	Application::SetShaderPath("../../Shaders");

	Application::Initialize("Hello, World", 0);

	TextRenderer* MyFont = new TextRenderer("../Font.ttf");

	UIText* HelloText = new UIText(1.0f, Vector3f32(1.0f), "Hello, World", MyFont);

	while (!Application::Quit)
	{
		Application::UpdateWindow();
	}
}

int WinMain()
{
	return main();
}