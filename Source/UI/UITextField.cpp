#include <kui/UI/UITextField.h>
#include <kui/UI/UIText.h>
#include <kui/Input.h>
#include "../Rendering/VertexBuffer.h"
#include "../Internal/OpenGL.h"
#include <kui/App.h>
#include <kui/Rendering/ScrollObject.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/Rendering/Shader.h>
#include <kui/Window.h>
#include <cmath>
#include <iostream>

using namespace kui;

void UITextField::Tick()
{
	TextObject->WrapDistance = Size.X - 0.01f;
	float CharSize = UIText::GetTextSizeAtScale(TextObject->GetTextSize(), TextObject->GetTextFont()).Y;
	if (TextObject->GetUsedSize().GetScreen().Y > Size.Y)
		TextScroll.MaxScroll = std::max(TextObject->GetUsedSize().GetScreen().Y - Size.Y + 0.025f, 0.0f);
	else
		TextScroll.MaxScroll = 0;
	TextObject->CurrentScrollObject = &this->TextRenderScroll;

	Vec2f Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	if (ParentWindow->UI.HoveredBox == this && !UIScrollBox::IsDraggingScrollBox)
	{
		if (ParentWindow->Input.IsLMBClicked)
		{
			ClickStartedOnField = true;
		}

		size_t Nearest = TextObject->GetNearestLetterAtLocation(ParentWindow->Input.MousePosition);
		if (!IsHovered)
		{
			RedrawElement();
		}
		IsHovered = true;

		if (ParentWindow->Input.IsLMBDown && ClickStartedOnField && !(!Dragging && ParentWindow->Input.PollForText && !IsEdited))
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
		ClickStartedOnField = false;
		Dragging = false;
	}

	if (ParentWindow->UI.KeyboardFocusBox == this && ParentWindow->Input.IsKeyDown(Key::RETURN))
	{
		this->Edit();
	}

	if (IsEdited)
	{
		ParentWindow->Input.TextAllowNewLine = AllowNewLine;
		ParentWindow->Input.CanEditText = CanEdit;
		EnteredText = ParentWindow->Input.Text;
		if (!ParentWindow->Input.PollForText)
		{
			IsEdited = false;
			if (OnChanged) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(OnChanged, nullptr, 0));
			RedrawElement();
		}
		if (!IsHovered && ParentWindow->Input.IsLMBDown && !Dragging)
		{
			IsEdited = false;
			ParentWindow->Input.PollForText = false;
			if (OnChanged) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(OnChanged, nullptr, 0));
			RedrawElement();
		}
	}

	std::string NewText = EnteredText.empty() && !IsEdited ? HintText : EnteredText;

	TextObject->SetColor(EnteredText.empty() && !IsEdited ? Vec3f::Lerp(TextColor, Color, 0.25f) : TextColor);

	if (NewText != TextObject->GetText())
	{
		TextObject->SetText(NewText);
		RedrawElement();
	}

	TextTimer += ParentWindow->GetDeltaTime();

	Vec2f EditedTextPos = IsEdited ? TextObject->GetLetterLocation(ParentWindow->Input.TextIndex) : 0;
	float CursorFraction = (OffsetPosition.Y - EditedTextPos.Y) / Size.Y + 1;

	if (!Dragging && IsEdited && ParentWindow->Input.TextSelectionStart == ParentWindow->Input.TextIndex)
		TextScroll.Percentage = std::max((CursorFraction - 1) * Size.Y + 0.025f, 0.0f);
	TextScroll.Percentage = std::min(TextScroll.Percentage, TextScroll.MaxScroll);

	if (EditedTextPos != IBeamPosition)
	{
		TextTimer = 0;
		IBeamPosition = EditedTextPos;
		IBeamScale = Vec2f(2.0f / ParentWindow->GetSize().X, CharSize);
		RedrawElement();
	}

	if (fmod(TextTimer, 1) < 0.5f && IsEdited)
	{
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
		TextHighlightStart = TextObject->GetLetterLocation(ParentWindow->Input.TextSelectionStart);
		TextHighlightEnd = IBeamPosition;
		if (TextHighlightStart.Y > TextHighlightEnd.Y)
			std::swap(TextHighlightStart, TextHighlightEnd);
		if (TextHighlightStart.Y == TextHighlightEnd.Y && TextHighlightStart.X > TextHighlightEnd.X)
			std::swap(TextHighlightStart, TextHighlightEnd);
	}
}

UITextField* UITextField::SetAllowNewLine(bool NewValue)
{
	this->AllowNewLine = NewValue;
	return this;
}

UITextField* UITextField::SetCanEdit(bool NewValue)
{
	this->CanEdit = NewValue;
	return this;
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

UITextField* UITextField::SetTextSize(UISize NewTextSize)
{
	TextObject->SetTextSize(NewTextSize);
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

UITextField* UITextField::SetColor(Vec3f NewColor)
{
	if (NewColor != Color)
	{
		Color = NewColor;
		ParentWindow->UI.RedrawUI();
	}
	return this;
}

Vec3f UITextField::GetColor() const
{
	return Color;
}

UITextField* UITextField::SetTextColor(Vec3f NewColor)
{
	TextColor = NewColor;
	return this;
}

Vec3f UITextField::GetTextColor()
{
	return TextObject->GetColor();
}

UISize UITextField::GetTextSize() const
{
	return TextObject->GetTextSize();
}

std::string UITextField::GetText()
{
	return EnteredText;
}

kui::UITextField* kui::UITextField::SetTextSizeMode(SizeMode Mode)
{
	return this;
}

UITextField::UITextField(Vec2f Position, Vec3f Color, Font* Renderer, std::function<void()> OnChanged)
	: UIBackground(true, Position, Color)
{
	TextFieldColor = Color;
	TextObject = new UIText(UISize::Pixels(11), Vec3f(1), HintText, Renderer);
	TextObject->SetPadding(UISize::Pixels(3));
	TextObject->Wrap = true;
	HasMouseCollision = true;
	KeyboardFocusable = true;
	this->OnChanged = OnChanged;
	AddChild(TextObject);
}

void UITextField::Edit()
{
	TextObject->WrapDistance = 15;
	IsEdited = true;
	ParentWindow->Input.PollForText = true;
	ParentWindow->Input.Text = EnteredText;
	IsPressed = false;
	ParentWindow->Input.SetTextIndex((int)EnteredText.size(), true);
	RedrawElement();
}

UITextField::~UITextField()
{
	if (IsEdited)
	{
		EnteredText = ParentWindow->Input.Text;
		ParentWindow->Input.PollForText = false;
	}
}

void UITextField::Update()
{
}

bool kui::UITextField::GetIsHovered() const
{
	return IsHovered;
}

bool kui::UITextField::GetIsPressed() const
{
	return IsPressed;
}

void UITextField::DrawBackground()
{
	TextScroll.Position = GetPosition();
	TextScroll.Scale = Vec2f(0) - Size;
	
	Vec2f Pos = Vec2f(TextScroll.Position.Y, TextScroll.Position.Y - TextScroll.Scale.Y);
	float Percentage = -TextScroll.Percentage;

	if (CurrentScrollObject)
	{
		Percentage -= CurrentScrollObject->Percentage;
		TextRenderScroll.Position = CurrentScrollObject->Position;
		TextRenderScroll.Scale = CurrentScrollObject->Scale;
		TextRenderScroll.Percentage = -Percentage;
		Pos = Vec2f(TextRenderScroll.Position.Y, TextRenderScroll.Position.Y - TextRenderScroll.Scale.Y);
	}
	else
	{
		TextRenderScroll.Position = TextScroll.Position;
		TextRenderScroll.Scale = TextScroll.Scale;
		TextRenderScroll.Percentage = TextScroll.Percentage;
	}
	BackgroundShader->Bind();
	BoxVertexBuffer->Bind();



	BackgroundShader->SetVec3("u_offset",
		Vec3f(Percentage, Pos.X, Pos.Y));

	if (IsEdited && ParentWindow->Input.TextSelectionStart != ParentWindow->Input.TextIndex)
	{
		float CharSize = IBeamScale.Y;
		auto DrawHighlight = [this, CharSize](Vec2f Start, Vec2f End)
			{
				BackgroundShader->SetVec3("u_color", Vec3f(0.25f, 1, 1));
				BackgroundShader->SetInt("u_drawCorner", 0);
				BackgroundShader->SetInt("u_drawBorder", 0);
				BackgroundShader->SetFloat("u_opacity", 0.5f);
				Vec2f Size = End - Start;
				glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"),
					Start.X, Start.Y, Size.X, Size.Y + CharSize
				);

				BoxVertexBuffer->Draw();
			};

		if (TextHighlightStart.Y == TextHighlightEnd.Y)
		{
			DrawHighlight(TextHighlightStart, TextHighlightEnd);
		}
		else
		{
			size_t Difference = size_t(std::round((TextHighlightEnd.Y - TextHighlightStart.Y) / CharSize));

			if (Difference == 1)
			{
				DrawHighlight(TextHighlightEnd, Vec2f(OffsetPosition.X + Size.X, TextHighlightEnd.Y));
				DrawHighlight(Vec2f(OffsetPosition.X, TextHighlightStart.Y), TextHighlightStart);
			}
			else
			{
				DrawHighlight(TextHighlightEnd, Vec2f(OffsetPosition.X + Size.X, TextHighlightEnd.Y));
				DrawHighlight(
					Vec2f(OffsetPosition.X, TextHighlightStart.Y + CharSize), 
					Vec2f(OffsetPosition.X + Size.X, TextHighlightEnd.Y - CharSize)
				);
				DrawHighlight(TextHighlightStart, Vec2f(OffsetPosition.X, TextHighlightStart.Y));
			}
		}
	}

	if (ShowIBeam)
	{
		BackgroundShader->SetVec3("u_color", TextColor);
		BackgroundShader->SetInt("u_drawCorner", 0);
		BackgroundShader->SetInt("u_drawBorder", 0);
		BackgroundShader->SetFloat("u_opacity", 1);
		glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"),
			IBeamPosition.X, IBeamPosition.Y, IBeamScale.X, IBeamScale.Y);
		BoxVertexBuffer->Draw();
	}

	BoxVertexBuffer->Unbind();

}