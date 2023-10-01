#include <UI/UIBackground.h>
#include <GL/glew.h>
#include <iostream>
#include "../Rendering/VertexBuffer.h"
#include "../Rendering/Shader.h"
#include <Application.h>
#include <Rendering/ScrollObject.h>

namespace UI
{
	Shader* UIShader = nullptr;
}

void UIBackground::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), -CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y);
	}
	else
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), 0, -1000, 1000);
}

void UIBackground::MakeGLBuffers(bool InvertTextureCoordinates)
{
	if(BoxVertexBuffer)
	delete BoxVertexBuffer;
	BoxVertexBuffer = new VertexBuffer(
		{
			Vertex(Vector2f32(0, 0), Vector2f32(0, 0)),
			Vertex(Vector2f32(0, 1), Vector2f32(0, 1)),
			Vertex(Vector2f32(1, 0), Vector2f32(1, 0)),
			Vertex(Vector2f32(1, 1), Vector2f32(1, 1))
		},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
}

bool UIBackground::GetRenderHighResMode()
{
	return UseTexture;
}

UIBackground* UIBackground::SetOpacity(float NewOpacity)
{
	if (NewOpacity != Opacity)
	{
		Opacity = NewOpacity;
		RedrawUI();
	}
	return this;
}

float UIBackground::GetOpacity()
{
	return Opacity;
}

void UIBackground::SetColor(Vector3f32 NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		RedrawUI();
	}
}

Vector3f32 UIBackground::GetColor()
{
	return Color;
}

void UIBackground::SetInvertTextureCoordinates(bool Invert)
{
	MakeGLBuffers(Invert);
}

UIBackground* UIBackground::SetUseTexture(bool UseTexture, unsigned int TextureID)
{
	if (this->UseTexture != UseTexture || TextureID != this->TextureID)
	{
		this->UseTexture = UseTexture;
		this->TextureID = TextureID;
		RedrawUI();
	}
	return this;
}

UIBackground::UIBackground(bool Horizontal, Vector2f Position, Vector3f32 Color, Vector2f MinScale) : UIBox(Horizontal, Position)
{
	SetMinSize(MinScale);
	this->Color = Color;
	if (UI::UIShader == nullptr) UI::UIShader = new Shader(Application::GetShaderPath() + "/uishader.vert", Application::GetShaderPath() + "/uishader.frag");
	this->BackgroundShader = UI::UIShader;
	MakeGLBuffers();
}

UIBackground::~UIBackground()
{
	delete BoxVertexBuffer;
}

void UIBackground::Draw()
{
	BackgroundShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	BoxVertexBuffer->Bind();
	ScrollTick(BackgroundShader);
	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_color"), Color.X, Color.Y, Color.Z, 1.f);
	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), OffsetPosition.X, OffsetPosition.Y, Size.X, Size.Y);
	glUniform1f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_opacity"), Opacity);
	glUniform1i(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_borderType"), (unsigned int)BoxBorder);
	glUniform1f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_borderScale"), BorderRadius / 20.0f);
	glUniform1f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_aspectratio"), Application::AspectRatio);
	if (UseTexture)
		glUniform1i(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_usetexture"), 1);
	else
		glUniform1i(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_usetexture"), 0);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	BoxVertexBuffer->Draw();
	BoxVertexBuffer->Unbind();
}

void UIBackground::Update()
{
}

void UIBackground::OnAttached()
{
}

UIBackgroundStyle::UIBackgroundStyle(std::string Name) : UIStyle("UIBackground: " + Name)
{
}

void UIBackgroundStyle::ApplyDerived(UIBox* Target)
{
	UIBackground* TargetBG = ToSafeElemPtr<UIBackground>(Target);

	TargetBG->SetColor(Color);
	TargetBG->SetOpacity(Opacity);
	TargetBG->SetUseTexture(UseTexture, TextureID);
}
