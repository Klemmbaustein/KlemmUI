#include <KlemmUI/UI/UITextField.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/Input.h>
#include "../Rendering/VertexBuffer.h"
#include "../MathHelpers.h"
#include <GL/glew.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Rendering/ScrollObject.h>
#include <KlemmUI/Rendering/Shader.h>
#include <KlemmUI/Window.h>
#include <cmath>

using namespace KlemmUI;


void UITextField::Tick()
{
	TextObject->WrapDistance = std::max(std::max(Size.X * 1.3f, GetMinSize().X), 0.1f) * 2;

	ButtonColorMultiplier = 1.f;
	Vector2f Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	if (ParentWindow->UI.HoveredBox == this)
	{
		size_t Nearest = TextObject->GetNearestLetterAtLocation(Window::GetActiveWindow()->Input.MousePosition);
		if (!IsHovered)
		{
			ParentWindow->UI.RedrawUI();
		}
		IsHovered = true;
		ButtonColorMultiplier = 0.8f;

		if (Window::GetActiveWindow()->Input.IsLMBDown && !(!Dragging && TextInput::PollForText && !IsEdited))
		{
			ButtonColorMultiplier = 0.5f;
			TextInput::PollForText = true;
			TextInput::Text = EnteredText;
			TextInput::SetTextIndex((int)Nearest, !Dragging);
			TextTimer = 0;
			DoubleClickTimer = 0;
			Dragging = true;
			IsEdited = true;
			if (!IsPressed)
			{
				ParentWindow->UI.RedrawUI();
				IsPressed = true;
			}
		}
		else if (IsPressed)
		{
			IsEdited = true;
			TextInput::PollForText = true;
			TextInput::Text = EnteredText;
			IsPressed = false;
			TextInput::TextIndex = Nearest;
			ParentWindow->UI.RedrawUI();
		}
	}
	else
	{
		if (IsPressed && !Dragging)
		{
			IsPressed = false;
		}
		if (IsHovered)
		{
			IsHovered = false;
			ParentWindow->UI.RedrawUI();
		}
	}

	if (!Window::GetActiveWindow()->Input.IsLMBDown)
	{
		Dragging = false;
	}

	if (IsEdited)
	{
		EnteredText = TextInput::Text;
		if (!TextInput::PollForText)
		{
			IsEdited = false;
			if (PressedFunc) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(PressedFunc, nullptr, nullptr, 0));
			ParentWindow->UI.RedrawUI();
		}
		if (!IsHovered && Window::GetActiveWindow()->Input.IsLMBDown && !Dragging)
		{
			IsEdited = false;
			TextInput::PollForText = false;
			if (PressedFunc) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(PressedFunc, nullptr, nullptr, 0));
			ParentWindow->UI.RedrawUI();
		}
	}
	std::string RenderedText = EnteredText;
	DoubleClickTimer += 0.01f;
	TextTimer += 0.01f;
	Vector2f EditedTextPos = IsEdited ? TextObject->GetLetterLocation(TextInput::TextIndex) : 0;

	if (fmod(TextTimer, 1) < 0.5f && IsEdited)
	{
		if (EditedTextPos != IBeamPosition)
		{
			TextTimer = 0;
			IBeamPosition = EditedTextPos;
			IBeamScale = Vector2f(2.0f / Window::GetActiveWindow()->GetSize().X, TextObject->GetUsedSize().Y);
			ParentWindow->UI.RedrawUI();
		}
		if (!ShowIBeam)
		{
			ParentWindow->UI.RedrawUI();
		}
		ShowIBeam = true;
	}
	else
	{
		if (ShowIBeam)
		{
			ParentWindow->UI.RedrawUI();
		}
		ShowIBeam = false;
	}

	if (IsEdited)
	{
		TextHighlightPos = TextObject->GetLetterLocation(TextInput::TextSelectionStart);
		TextHighlightSize = Vector2f(std::abs(EditedTextPos.X - TextHighlightPos.X), TextObject->GetUsedSize().Y);

		float MinX = std::min(EditedTextPos.X, TextHighlightPos.X);
		TextHighlightPos.X = MinX;
	}
	TextObject->SetColor(EnteredText.empty() && !IsEdited ? TextColor * 0.75 : TextColor);
	TextObject->SetText(EnteredText.empty() && !IsEdited ? HintText : (IsEdited ? RenderedText : EnteredText));
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

UITextField* UITextField::SetTextRenderer(TextRenderer* Font)
{
	TextObject->SetTextRenderer(Font);
	return this;
}

UITextField* UITextField::SetHintText(std::string NewHintText)
{
	HintText = NewHintText;
	return this;
}

UITextField* UITextField::SetColor(Vector3f NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		ParentWindow->UI.RedrawUI();
	}
	return this;
}

Vector3f UITextField::GetColor()
{
	return Color;
}

UITextField* UITextField::SetTextColor(Vector3f NewColor)
{
	TextColor = NewColor;
	return this;
}

Vector3f UITextField::GetTextColor()
{
	return TextObject->GetColor();
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

UITextField::UITextField(bool Horizontal, Vector2f Position, Vector3f Color, TextRenderer* Renderer, void(*PressedFunc)())
	: UIBackground(Horizontal, Position, Color)
{
	TextObject = new UIText(0, Vector3(1), HintText, Renderer);
	TextObject->SetTextSize(0.5f);
	TextObject->SetPadding(0.005f);
	TextObject->Wrap = true;
	HasMouseCollision = true;
	this->PressedFunc = PressedFunc;
	AddChild(TextObject);
}

UITextField::~UITextField()
{
	IsEdited = false;
	EnteredText = TextInput::Text;
	TextInput::PollForText = false;
}

void UITextField::Update()
{
}

void UITextField::DrawBackground()
{
	BackgroundShader->Bind();
	BoxVertexBuffer->Bind();

	if (IsEdited)
	{
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_color"), 0, 0.25f, 1, 1);
		BackgroundShader->SetInt("u_borderType", (int)UIBox::BorderType::None);
		BackgroundShader->SetFloat("u_opacity", 0.5f);
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), TextHighlightPos.X, TextHighlightPos.Y, TextHighlightSize.X, TextHighlightSize.Y);
		BoxVertexBuffer->Draw();
	}

	if (ShowIBeam)
	{
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_color"), TextColor.X, TextColor.Y, TextColor.Z, 1);
		BackgroundShader->SetInt("u_borderType", (int)UIBox::BorderType::None);
		BackgroundShader->SetFloat("u_opacity", 1);
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), IBeamPosition.X, IBeamPosition.Y, IBeamScale.X, IBeamScale.Y);
		BoxVertexBuffer->Draw();
	}

	BoxVertexBuffer->Unbind();

}