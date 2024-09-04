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

	Vector2f Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	if (ParentWindow->UI.HoveredBox == this)
	{
		size_t Nearest = TextObject->GetNearestLetterAtLocation(ParentWindow->Input.MousePosition);
		if (!IsHovered)
		{
			RedrawElement();
		}
		IsHovered = true;

		if (ParentWindow->Input.IsLMBDown && !(!Dragging && ParentWindow->Input.PollForText && !IsEdited))
		{
			ParentWindow->Input.PollForText = true;
			ParentWindow->Input.Text = EnteredText;
			ParentWindow->Input.SetTextIndex((int)Nearest, !Dragging);
			TextTimer = 0;
			Dragging = true;
			IsEdited = true;
			if (!IsPressed)
			{
				RedrawElement();
				IsPressed = true;
			}
		}
		else if (IsPressed)
		{
			IsEdited = true;
			ParentWindow->Input.PollForText = true;
			ParentWindow->Input.Text = EnteredText;
			IsPressed = false;
			ParentWindow->Input.TextIndex = (int)Nearest;
			RedrawElement();
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
			RedrawElement();
		}
	}

	if (!ParentWindow->Input.IsLMBDown)
	{
		Dragging = false;
	}

	if (IsEdited)
	{
		EnteredText = ParentWindow->Input.Text;
		if (!ParentWindow->Input.PollForText)
		{
			IsEdited = false;
			if (OnClickedFunction) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(OnClickedFunction, nullptr, nullptr, 0));
			RedrawElement();
		}
		if (!IsHovered && ParentWindow->Input.IsLMBDown && !Dragging)
		{
			IsEdited = false;
			ParentWindow->Input.PollForText = false;
			if (OnClickedFunction) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(OnClickedFunction, nullptr, nullptr, 0));
			RedrawElement();
		}
	}
	std::string RenderedText = EnteredText;
	TextTimer += ParentWindow->GetDeltaTime();
	Vector2f EditedTextPos = IsEdited ? TextObject->GetLetterLocation(ParentWindow->Input.TextIndex) : 0;

	if (fmod(TextTimer, 1) < 0.5f && IsEdited)
	{
		if (EditedTextPos != IBeamPosition)
		{
			TextTimer = 0;
			IBeamPosition = EditedTextPos;
			IBeamScale = Vector2f(2.0f / ParentWindow->GetSize().X, TextObject->GetUsedSize().Y);
			RedrawElement();
		}
		if (!ShowIBeam)
		{
			RedrawElement();
		}
		ShowIBeam = true;
	}
	else
	{
		if (ShowIBeam)
		{
			RedrawElement();
		}
		ShowIBeam = false;
	}

	if (IsEdited)
	{
		TextHighlightPos = TextObject->GetLetterLocation(ParentWindow->Input.TextSelectionStart);
		TextHighlightSize = Vector2f(std::abs(EditedTextPos.X - TextHighlightPos.X), TextObject->GetUsedSize().Y);

		float MinX = std::min(EditedTextPos.X, TextHighlightPos.X);
		TextHighlightPos.X = MinX;
	}
	TextObject->SetColor(EnteredText.empty() && !IsEdited ? Vector3f::Lerp(TextColor, Color, 0.25f) : TextColor);
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
			ParentWindow->Input.Text = NewText;
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

UITextField* UITextField::SetFont(Font* Font)
{
	TextObject->SetFont(Font);
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

Vector3f UITextField::GetColor() const
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

float UITextField::GetTextSize() const
{
	return TextSize;
}

std::string UITextField::GetText()
{
	return EnteredText;
}

KlemmUI::UITextField* KlemmUI::UITextField::SetTextSizeMode(UIBox::SizeMode Mode)
{
	TextObject->SetTextSizeMode(Mode);
	return this;
}

UITextField::UITextField(Vector2f Position, Vector3f Color, Font* Renderer, std::function<void()> OnClickedFunction)
	: UIBackground(true, Position, Color)
{
	TextFieldColor = Color;
	TextObject = new UIText(0, Vector3f(1), HintText, Renderer);
	TextObject->SetTextSize(0.5f);
	TextObject->SetPadding(0.005f);
	TextObject->Wrap = true;
	HasMouseCollision = true;
	this->OnClickedFunction = OnClickedFunction;
	AddChild(TextObject);
}

UITextField::~UITextField()
{
	IsEdited = false;
	EnteredText = ParentWindow->Input.Text;
	ParentWindow->Input.PollForText = false;
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
		BackgroundShader->SetVec3("u_color", Vector3f(0.25f, 1, 1));
		BackgroundShader->SetInt("u_drawCorner", 0);
		BackgroundShader->SetInt("u_drawBorder", 0);
		BackgroundShader->SetFloat("u_opacity", 0.5f);
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), TextHighlightPos.X, TextHighlightPos.Y, TextHighlightSize.X, TextHighlightSize.Y);
		BoxVertexBuffer->Draw();
	}

	if (ShowIBeam)
	{
		BackgroundShader->SetVec3("u_color", TextColor);
		BackgroundShader->SetInt("u_drawCorner", 0);
		BackgroundShader->SetInt("u_drawBorder", 0);
		BackgroundShader->SetFloat("u_opacity", 1);
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), IBeamPosition.X, IBeamPosition.Y, IBeamScale.X, IBeamScale.Y);
		BoxVertexBuffer->Draw();
	}

	BoxVertexBuffer->Unbind();

}