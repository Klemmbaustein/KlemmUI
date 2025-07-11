#include <kui/UI/UIBackground.h>
#include "../Internal/OpenGL.h"
#include "../Rendering/VertexBuffer.h"
#include <kui/Rendering/Shader.h>
#include <kui/App.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/Window.h>
#include <iostream>
using namespace kui;

thread_local VertexBuffer* UIBackground::BoxVertexBuffer = nullptr;

void UIBackground::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
		UsedShader->SetVec3("u_offset",
			Vec3f(-CurrentScrollObject->GetOffset(), CurrentScrollObject->GetPosition().Y, CurrentScrollObject->GetScale().Y));
	else
		UsedShader->SetVec3("u_offset", Vec3f(0, -1000, 1000));
}

void UIBackground::MakeGLBuffers()
{
	if (!BoxVertexBuffer)
	{
		BoxVertexBuffer = new VertexBuffer(
			{
				Vertex(Vec2f(0, 0), 0),
				Vertex(Vec2f(0, 1), 1),
				Vertex(Vec2f(1, 0), 2),
				Vertex(Vec2f(1, 1), 3)
			},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
	}
}

void UIBackground::DrawBackground()
{
}

UIBackground* UIBackground::SetBorderEdges(bool Top, bool Down, bool Left, bool Right)
{
	int NewFlags = uint8_t(Right) | uint8_t(Left) << 1 | uint8_t(Top) << 2 | uint8_t(Down) << 3;
	if (NewFlags != BorderFlags)
	{
		BorderFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetCorners(bool TopLeft, bool TopRight, bool BottomLeft, bool BottomRight)
{
	int NewFlags = uint8_t(BottomLeft) | uint8_t(TopLeft) << 1 | uint8_t(BottomRight) << 2 | uint8_t(TopRight) << 3;
	if (NewFlags != CornerFlags)
	{
		CornerFlags = NewFlags;
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

void kui::UIBackground::FreeVertexBuffer()
{
	if (BoxVertexBuffer)
		delete BoxVertexBuffer;
	BoxVertexBuffer = nullptr;
}

UIBackground* UIBackground::SetOpacity(float NewOpacity)
{
	if (NewOpacity != Opacity)
	{
		Opacity = NewOpacity;
		RedrawElement();
	}
	return this;
}

UIBackground* kui::UIBackground::SetBorderColor(Vec3f NewColor)
{
	if (NewColor != BorderColor)
	{
		BorderColor = NewColor;
		RedrawElement();
	}
	return this;
}

float UIBackground::GetOpacity() const
{
	return Opacity;
}

UIBackground* UIBackground::SetColor(Vec3f NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		RedrawElement();
	}
	return this;
}

Vec3f UIBackground::GetColor() const
{
	return Color;
}

UIBackground* UIBackground::SetUseTexture(bool UseTexture, unsigned int TextureID)
{
	if (OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(TextureID);
		OwnsTexture = false;
	}

	if (this->UseTexture != UseTexture || TextureID != this->TextureID)
	{
		this->UseTexture = UseTexture;
		this->TextureID = TextureID;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetBorder(UISize BorderSize, Vec3f Color)
{
	if (BorderSize != this->BorderRadius)
	{
		this->BorderRadius = BorderSize;
		RedrawElement();
	}
	SetBorderColor(Color);
	return this;
}

UIBackground* UIBackground::SetCorner(UISize CornerSize)
{
	if (CornerSize != CornerRadius)
	{
		CornerRadius = CornerSize;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetCornerVisible(int Index, bool Value)
{
	int NewFlags = CornerFlags & ~(1 << Index);

	if (Value)
		NewFlags |= 1 << Index;

	if (NewFlags != CornerFlags)
	{
		CornerFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetBorderVisible(int Index, bool Value)
{
	int NewFlags = BorderFlags & ~(1 << Index);

	if (Value)
		NewFlags |= 1 << Index;

	if (NewFlags != BorderFlags)
	{
		BorderFlags = NewFlags;
		RedrawElement();
	}
	return this;
}

UIBackground* kui::UIBackground::SetUseTexture(bool UseTexture, std::string TextureFile)
{
	if (OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(TextureID);
		OwnsTexture = false;
	}

	if (TextureFile.empty())
	{
		UseTexture = false;
	}

	unsigned int NewTextureID = 0;
	if (UseTexture)
	{
		NewTextureID = ParentWindow->UI.LoadReferenceTexture(TextureFile);
		OwnsTexture = true;
	}
	if (this->UseTexture != UseTexture || NewTextureID != this->TextureID)
	{
		this->UseTexture = UseTexture;
		this->TextureID = NewTextureID;
		RedrawElement();
	}

	return this;
}

UIBackground::UIBackground(bool Horizontal, Vec2f Position, Vec3f Color, SizeVec MinScale, Shader* UsedShader) : UIBox(Horizontal, Position)
{
	SetMinSize(MinScale);
	this->Color = Color;
	if (!UsedShader)
	{
		this->BackgroundShader = Window::GetActiveWindow()->Shaders.LoadShader("res:shaders/uishader.vert", "res:shaders/uishader.frag", "UI Shader");
	}
	else
	{
		this->BackgroundShader = UsedShader;
	}
	MakeGLBuffers();
}

UIBackground::~UIBackground()
{
	if (OwnsTexture)
	{
		ParentWindow->UI.UnloadReferenceTexture(TextureID);
		OwnsTexture = false;
	}
}

void UIBackground::Draw()
{
	if (!BoxVertexBuffer)
	{
		return;
	}
	BackgroundShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	BoxVertexBuffer->Bind();
	ScrollTick(BackgroundShader);
	BackgroundShader->SetVec3("u_color", Color);
	BackgroundShader->SetVec3("u_borderColor", BorderColor);

	UISize DrawnBorderRadius = BorderRadius;

	if (this == ParentWindow->UI.KeyboardFocusBox)
	{
		if (DrawnBorderRadius.Value == 0)
		{
			DrawnBorderRadius = 2_px;
		}
		else
		{
			DrawnBorderRadius.Value *= 2;
		}
	}

	glUniform4f(BackgroundShader->GetUniformLocation("u_transform"), OffsetPosition.X, OffsetPosition.Y, Size.X, Size.Y);
	BackgroundShader->SetFloat("u_opacity", Opacity);
	BackgroundShader->SetInt("u_drawBorder", DrawnBorderRadius.Value != 0);
	BackgroundShader->SetInt("u_drawCorner", CornerRadius.Value != 0);
	BackgroundShader->SetFloat("u_borderScale", GetBorderSize(DrawnBorderRadius));
	BackgroundShader->SetFloat("u_cornerScale", GetBorderSize(CornerRadius));
	BackgroundShader->SetInt("u_cornerFlags", int(CornerFlags));
	BackgroundShader->SetInt("u_borderFlags", int(BorderFlags));
	BackgroundShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());
	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));

	BackgroundShader->SetInt("u_useTexture", (int)UseTexture);
	BoxVertexBuffer->Draw();
	DrawBackground();
	BoxVertexBuffer->Unbind();
}

void UIBackground::Update()
{
}

void UIBackground::OnAttached()
{
}
