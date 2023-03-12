#include <UI/UITextField.h>
#include <UI/UIText.h>
#include <Math/MathHelpers.h>
#include <Input.h>
#include "../Rendering/VertexBuffer.h"
#include <GL/glew.h>
#include <Application.h>
#include <Rendering/ScrollObject.h>
#include "../Rendering/Shader.h"
#include <iostream>

namespace UI
{
	extern Shader* UIShader;
}

void UITextField::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"),
			-CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y, CurrentScrollObject->Position.Y - CurrentScrollObject->Scale.Y);
	}
	else
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), 0, -1000, 1000);
}

void UITextField::MakeGLBuffers()
{
	if (ButtonVertexBuffer)
		delete ButtonVertexBuffer;
	ButtonVertexBuffer = new VertexBuffer(
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

void UITextField::Tick()
{
	ButtonColorMultiplier = 1.f;
	Vector2f Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	if (Math::IsPointIn2DBox(OffsetPosition + Offset, OffsetPosition + Size + Offset, Input::MouseLocation))
	{
		if (!IsHovered)
		{
			RedrawUI();
		}
		IsHovered = true;
		ButtonColorMultiplier = 0.8f;
		if (Input::IsLMBDown)
		{
			ButtonColorMultiplier = 0.5f;
			if (!IsPressed)
			{
				RedrawUI();
				IsPressed = true;
			}
		}
		else if (IsPressed)
		{
			IsEdited = true;
			TextInput::PollForText = true;
			TextInput::Text = EnteredText;
			IsPressed = false;
			TextInput::TextIndex = TextInput::Text.size();
			RedrawUI();
		}
	}
	else
	{
		if (IsPressed)
		{
			IsPressed = false;
		}
		if (IsHovered)
		{
			IsHovered = false;
			RedrawUI();
		}
	}
	if (IsEdited)
	{
		EnteredText = TextInput::Text;
		if (!TextInput::PollForText)
		{
			IsEdited = false;
			if (PressedFunc) Application::ButtonEvents.push_back(Application::ButtonEvent(PressedFunc));
			RedrawUI();
		}
		if (!IsHovered && Input::IsLMBDown)
		{
			IsEdited = false;
			TextInput::PollForText = false;
			if (PressedFunc) Application::ButtonEvents.push_back(Application::ButtonEvent(PressedFunc));
			RedrawUI();
		}
	}
	std::string RendererdText = EnteredText;
	if ((TextInput::TextIndex == TextInput::Text.size()) && fmod(Application::Time, 1) < 0.5f && IsEdited)
	{
		RendererdText.append("_");
	}
	else if (fmod(Application::Time, 1) < 0.5f && IsEdited)
	{
		RendererdText.at(TextInput::TextIndex) = '|';
	}
	TextObject->SetColor(EnteredText.empty() && !IsEdited ? Vector3f32(0.75) : Vector3f32(1));
	TextObject->SetText(EnteredText.empty() && !IsEdited ? HintText : (IsEdited ? RendererdText : EnteredText));
}

UITextField* UITextField::SetText(std::string NewText)
{
	if (NewText != EnteredText)
	{
		EnteredText = NewText;
		GetAbsoluteParent()->InvalidateLayout();
		if (IsEdited)
		{
			TextInput::Text = NewText;
		}
	}
	return this;
}

UITextField* UITextField::SetTextSize(float NewTextSize)
{
	if (NewTextSize != TextSize)
	{
		TextObject->SetTextSize(NewTextSize);
		TextSize = NewTextSize;
	}
	return this;
}

UITextField* UITextField::SetHintText(std::string NewHintText)
{
	HintText = NewHintText;
	return this;
}

float UITextField::GetTextSize()
{
	return TextSize;
}

std::string UITextField::GetText()
{
	return EnteredText;
}

bool UITextField::GetIsHovered()
{
	return IsHovered;
}

bool UITextField::GetIsPressed()
{
	return IsPressed;
}

UITextField::UITextField(bool Horizontal, Vector2f Position, Vector3f32 Color, TextRenderer* Renderer, void(*PressedFunc)())
	: UIBox(Horizontal, Position)
{
	if (UI::UIShader == nullptr) UI::UIShader = new Shader("Shaders/uishader.vert", "Shaders/uishader.frag");
	this->Color = Color;
	TextObject = new UIText(0, Vector3(1), HintText, Renderer);
	TextObject->SetTextSize(0.5);
	TextObject->SetPadding(0.005);
	TextObject->SetTryFill(true);
	TextObject->Wrap = true;
	this->PressedFunc = PressedFunc;
	AddChild(TextObject);
	MakeGLBuffers();
	//SetMinSize(Vector2(0.1, 0.04));
}

UITextField::~UITextField()
{
	IsEdited = false;
	EnteredText = TextInput::Text;
	TextInput::PollForText = false;
	delete ButtonVertexBuffer;
}

void UITextField::Draw()
{
	UI::UIShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	ButtonVertexBuffer->Bind();
	ScrollTick(UI::UIShader);
	glUniform4f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_transform"), OffsetPosition.X, OffsetPosition.Y, Size.X, Size.Y);
	glUniform4f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_color"),
		ButtonColorMultiplier * Color.X, ButtonColorMultiplier * Color.Y, ButtonColorMultiplier * Color.Z, 1.f);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_opacity"), 1.0);
	glUniform1i(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_usetexture"), 0);
	glUniform1i(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_borderType"), BorderType);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_borderScale"), BorderRadius / 20.0f);
	glUniform1f(glGetUniformLocation(UI::UIShader->GetShaderID(), "u_aspectratio"), Application::AspectRatio);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	ButtonVertexBuffer->Unbind();
}

void UITextField::Update()
{
	if (!TryFill)
	{
		TextObject->WrapDistance = std::max(std::max(TextObject->GetUsedSize().X, GetMinSize().X), 0.1);
		//Vector2f TextDesiredSize = TextObject->GetUsedSize();
		//TextDesiredSize += Vector2f(0.005);
		//Size = TextDesiredSize;
	}
	TextObject->WrapDistance = std::max(std::max(Size.X, GetMinSize().X), 0.1);
	TextObject->Update();
}