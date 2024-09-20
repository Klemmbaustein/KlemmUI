#include <kui/UI/UIBackground.h>
#include <GL/glew.h>
#include <iostream>
#include "../Rendering/VertexBuffer.h"
#include <kui/Rendering/Shader.h>
#include <kui/App.h>
#include <kui/Rendering/ScrollObject.h>
#include <kui/Window.h>
using namespace kui;

void UIBackground::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), -CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y);
	}
	else
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), 0, -1000, 1000);
}

void UIBackground::MakeGLBuffers()
{
	if (BoxVertexBuffer)
		delete BoxVertexBuffer;
	BoxVertexBuffer = new VertexBuffer(
		{
			Vertex(Vec2f(0, 0), Vec2f(0, 0), 0),
			Vertex(Vec2f(0, 1), Vec2f(0, 1.0001f), 1),
			Vertex(Vec2f(1, 0), Vec2f(1.0001f, 0), 2),
			Vertex(Vec2f(1, 1), Vec2f(1.0001f, 1.0001f), 3)
		},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
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

float kui::UIBackground::GetBorderSize(float InSize, UIBox::SizeMode Mode)
{
	switch (Mode)
	{
	case kui::UIBox::SizeMode::ScreenRelative:
	case kui::UIBox::SizeMode::AspectRelative:
		return InSize;
		break;
	case kui::UIBox::SizeMode::PixelRelative:
		return (std::floor(InSize * Window::GetActiveWindow()->GetDPI()) / (float)Window::GetActiveWindow()->GetSize().Y * 4.0f);
	default:
		return 0.0f;
	}
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

UIBackground* UIBackground::SetBorder(float Size, UIBox::SizeMode BorderSize)
{
	if (BorderSize != BorderSizeMode || Size != BorderRadius)
	{
		BorderSizeMode = BorderSize;
		BorderRadius = Size;
		RedrawElement();
	}
	return this;
}

UIBackground* UIBackground::SetCorner(float Size, UIBox::SizeMode BorderSize)
{
	if (BorderSize != CornerSizeMode || Size != CornerRadius)
	{
		CornerSizeMode = BorderSize;
		CornerRadius = Size;
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

UIBackground::UIBackground(bool Horizontal, Vec2f Position, Vec3f Color, Vec2f MinScale, Shader* UsedShader) : UIBox(Horizontal, Position)
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
	delete BoxVertexBuffer;
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

	Vec2ui WindowSize = ParentWindow->GetSize() / 2;

	Vec2f Pos = Vec2f(Vec2i(OffsetPosition * WindowSize)) / WindowSize;
	Vec2f Res = Vec2f(Vec2i(Size * WindowSize)) / WindowSize;

	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), Pos.X, Pos.Y, Res.X, Res.Y);
	BackgroundShader->SetFloat("u_opacity", Opacity);
	BackgroundShader->SetInt("u_drawBorder", BorderRadius != 0);
	BackgroundShader->SetInt("u_drawCorner", CornerRadius != 0);
	BackgroundShader->SetFloat("u_borderScale", GetBorderSize(BorderRadius, BorderSizeMode));
	BackgroundShader->SetFloat("u_cornerScale", GetBorderSize(CornerRadius, CornerSizeMode));
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
