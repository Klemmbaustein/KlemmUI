#include <KlemmUI/UI/UIBackground.h>
#include <GL/glew.h>
#include <iostream>
#include "../Rendering/VertexBuffer.h"
#include <KlemmUI/Rendering/Shader.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Rendering/ScrollObject.h>
#include <KlemmUI/Window.h>
using namespace KlemmUI;

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
			Vertex(Vector2f(0, 0), Vector2f(0, 0), 0),
			Vertex(Vector2f(0, 1), Vector2f(0, 1.01f), 1),
			Vertex(Vector2f(1, 0), Vector2f(1.001f, 0), 2),
			Vertex(Vector2f(1, 1), Vector2f(1.001f, 1.01f), 3)
		},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
}

void UIBackground::DrawBackground()
{
}

float KlemmUI::UIBackground::GetBorderSize(float InSize, UIBox::SizeMode Mode)
{
	switch (Mode)
	{
	case KlemmUI::UIBox::SizeMode::ScreenRelative:
	case KlemmUI::UIBox::SizeMode::AspectRelative:
		return InSize;
		break;
	case KlemmUI::UIBox::SizeMode::PixelRelative:
		return (InSize / (float)Window::GetActiveWindow()->GetSize().Y * Window::GetActiveWindow()->GetDPI()) * 4.0f;
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

UIBackground* KlemmUI::UIBackground::SetBorderColor(Vector3f NewColor)
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

UIBackground* UIBackground::SetColor(Vector3f NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		RedrawElement();
	}
	return this;
}

Vector3f UIBackground::GetColor() const
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

UIBackground* KlemmUI::UIBackground::SetUseTexture(bool UseTexture, std::string TextureFile)
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

UIBackground::UIBackground(bool Horizontal, Vector2f Position, Vector3f Color, Vector2f MinScale, Shader* UsedShader) : UIBox(Horizontal, Position)
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

	Vector2ui WindowSize = ParentWindow->GetSize() / 2;

	Vector2f Pos = Vector2f(Vector2i(OffsetPosition * WindowSize)) / WindowSize;
	Vector2f Res = Vector2f(Vector2i(Size * WindowSize)) / WindowSize;

	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), Pos.X, Pos.Y, Res.X, Res.Y);
	BackgroundShader->SetFloat("u_opacity", Opacity);
	BackgroundShader->SetInt("u_drawBorder", BorderRadius != 0);
	BackgroundShader->SetInt("u_drawCorner", CornerRadius != 0);
	BackgroundShader->SetFloat("u_borderScale", GetBorderSize(BorderRadius, BorderSizeMode));
	BackgroundShader->SetFloat("u_cornerScale", GetBorderSize(CornerRadius, CornerSizeMode));
	BackgroundShader->SetInt("u_cornerFlags", int(CornerFlags));
	BackgroundShader->SetInt("u_borderFlags", int(BorderFlags));
	BackgroundShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());
	BackgroundShader->SetVec2("u_screenRes", Vector2f(
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
