#include <kui/UI/UIBackground.h>
#include "../Rendering/VertexBuffer.h"
#include <kui/Rendering/Shader.h>
#include <kui/App.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/Rendering/OpenGLBackend.h>
#include <kui/Window.h>
#include <kui/Rendering/RenderBackend.h>
#include <iostream>
using namespace kui;

void UIBackground::MakeGLBuffers()
{
}

void UIBackground::DrawBackground(render::RenderBackend* Backend)
{
}

UIBackground* UIBackground::SetBorderEdges(bool Top, bool Down, bool Left, bool Right)
{
	int NewFlags = uint8_t(Right) | uint8_t(Left) << 1 | uint8_t(Top) << 2 | uint8_t(Down) << 3;
	if (NewFlags != State->BorderFlags)
	{
		State->BorderFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetCorners(bool TopLeft, bool TopRight, bool BottomLeft, bool BottomRight)
{
	int NewFlags = uint8_t(BottomLeft) | uint8_t(TopLeft) << 1 | uint8_t(BottomRight) << 2 | uint8_t(TopRight) << 3;
	if (NewFlags != State->CornerFlags)
	{
		State->CornerFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

float kui::UIBackground::GetBorderSize(UISize InSize)
{
	switch (InSize.Mode)
	{
	case kui::SizeMode::ScreenRelative:
	case kui::SizeMode::AspectRelative:
		return InSize.Value;
		break;
	case kui::SizeMode::PixelRelative:
		return (std::floor(InSize.Value * Window::GetActiveWindow()->GetDPI()) / (float)Window::GetActiveWindow()->GetSize().Y * 4.0f);
	default:
		return 0.0f;
	}
}

UIBackground* UIBackground::SetOpacity(float NewOpacity)
{
	if (NewOpacity != State->Opacity)
	{
		State->Opacity = NewOpacity;
		RedrawElement();
	}
	return this;
}

UIBackground* kui::UIBackground::SetBorderColor(Vec3f NewColor)
{
	if (NewColor != State->BorderColor)
	{
		State->BorderColor = NewColor;
		RedrawElement();
	}
	return this;
}

float UIBackground::GetOpacity() const
{
	return State->Opacity;
}

UIBackground* UIBackground::SetColor(Vec3f NewColor)
{
	if (NewColor != State->Color)
	{
		State->Color = NewColor;
		RedrawElement();
	}
	return this;
}

Vec3f UIBackground::GetColor() const
{
	return State->Color;
}

UIBackground* UIBackground::SetUseTexture(bool UseTexture, unsigned int TextureID)
{
	if (State->OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(TextureID);
		State->OwnsTexture = false;
	}

	if (State->UseTexture != UseTexture || TextureID != State->TextureID)
	{
		State->UseTexture = UseTexture;
		State->TextureID = TextureID;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetBorder(UISize BorderSize, Vec3f Color)
{
	if (BorderSize != State->BorderRadius)
	{
		State->BorderRadius = BorderSize;
		RedrawElement();
	}
	SetBorderColor(Color);
	return this;
}

UIBackground* UIBackground::SetCorner(UISize CornerSize)
{
	if (CornerSize != State->CornerRadius)
	{
		State->CornerRadius = CornerSize;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetCornerVisible(int Index, bool Value)
{
	int NewFlags = State->CornerFlags & ~(1 << Index);

	if (Value)
		NewFlags |= 1 << Index;

	if (NewFlags != State->CornerFlags)
	{
		State->CornerFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetBorderVisible(int Index, bool Value)
{
	int NewFlags = State->BorderFlags & ~(1 << Index);

	if (Value)
		NewFlags |= 1 << Index;

	if (NewFlags != State->BorderFlags)
	{
		State->BorderFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* kui::UIBackground::SetUseTexture(bool UseTexture, std::string TextureFile)
{
	if (State->OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(State->TextureID);
		State->OwnsTexture = false;
	}

	if (TextureFile.empty())
	{
		UseTexture = false;
	}

	unsigned int NewTextureID = 0;
	if (UseTexture)
	{
		NewTextureID = ParentWindow->UI.LoadReferenceTexture(TextureFile);
		State->OwnsTexture = true;
	}
	if (State->UseTexture != UseTexture || NewTextureID != State->TextureID)
	{
		State->UseTexture = UseTexture;
		State->TextureID = NewTextureID;
		RedrawElement();
	}

	return this;
}

UIBackground::UIBackground(bool Horizontal, Vec2f Position, Vec3f Color, SizeVec MinScale, Shader* UsedShader) : UIBox(Horizontal, Position)
{
	this->BackgroundShader = UsedShader;
	State = Window::GetActiveWindow()->UI.Render->MakeBackground();
	SetMinSize(MinScale);
	State->Color = Color;
}

UIBackground::~UIBackground()
{
	if (State->OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(State->TextureID);
		State->OwnsTexture = false;
	}
	delete State;
}

void UIBackground::Draw(render::RenderBackend* Backend)
{
	if (State)
	{
		auto GLBackend = dynamic_cast<render::OpenGLBackend*>(Backend);
		if (GLBackend && this->BackgroundShader)
		{
			static_cast<render::GLUIBackgroundState*>(State)->UsedShader = BackgroundShader;
		}
		State->IsHighlighted = this == ParentWindow->UI.KeyboardFocusBox;
		State->Draw(Backend, OffsetPosition, Size, CurrentScrollObject, &ParentWindow->Colors);
	}
	DrawBackground(Backend);
}

void UIBackground::Update()
{
}

void UIBackground::OnAttached()
{
}
