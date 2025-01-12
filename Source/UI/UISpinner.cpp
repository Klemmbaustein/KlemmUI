#include <kui/UI/UISpinner.h>
#include <kui/Window.h>
#include <iostream>
using namespace kui;

UISpinner::UISpinner(Vec2f Position, Vec3f Color, SizeVec Size)
	: UIBackground(true, Position, Color, Size, Window::GetActiveWindow()->Shaders.LoadShader(
		"res:shaders/uishader.vert",
		"res:shaders/spinner.frag",
		"Spinner shader"
	))
{
	SetMaxSize(Size);
}

UISpinner::~UISpinner()
{
}

UISpinner* UISpinner::SetSpeed(float NewSpeed)
{
	Speed = NewSpeed;
	return this;
}
UISpinner* UISpinner::SetBackgroundColor(Vec3f NewColor)
{
	if (NewColor != BackgroundColor)
	{
		BackgroundColor = NewColor;
		if (!Active)
			RedrawElement();
	}
	return this;
}

void UISpinner::Tick()
{
	if (Active)
	{
		Time += ParentWindow->GetDeltaTime() * Speed;
		RedrawElement();
	}
}

void UISpinner::Draw()
{
	BackgroundShader->Bind();
	BackgroundShader->SetFloat("u_time", ParentWindow->Time);
	BackgroundShader->SetVec3("u_backgroundColor", BackgroundColor);
	UIBackground::Draw();
}