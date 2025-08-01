#include <kui/UI/UITextField.h>
#include <kui/UI/UIText.h>
#include <kui/Input.h>
#include <kui/Rendering/OpenGLBackend.h>
#include "../Rendering/OpenGL.h"
#include <kui/App.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/Rendering/Shader.h>
#include <kui/Window.h>
#include <cmath>
#include <iostream>

using namespace kui;

void UITextField::Tick()
{
	if (Size.X > 0)
	{
		TextObject->WrapDistance = UISize::Screen(Size.X - 0.01f);
		TextObject->Wrap = true;
	}
	else
	{
		TextObject->Wrap = false;
	}

	if (TextObject->GetUsedSize().GetScreen().Y > Size.Y)
		TextScroll.MaxScroll = std::max(TextObject->GetUsedSize().GetScreen().Y - Size.Y + 0.025, 0.0);
	else
		TextScroll.MaxScroll = 0;
	TextObject->CurrentScrollObject = &this->TextScroll;
	TextScroll.Parent = CurrentScrollObject;
	if (CurrentScrollObject)
		TextObject->IsVisible = false;

	Vec2f Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->GetOffset();
	}
	if (ParentWindow->UI.HoveredBox == this && !UIScrollBox::IsDraggingScrollBox)
	{
		if (ParentWindow->Input.IsLMBClicked)
		{
			ClickStartedOnField = true;
		}

		std::size_t Nearest = TextObject->GetNearestLetterAtLocation(ParentWindow->Input.MousePosition);
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

	TextObject->SetColor(EnteredText.empty() && !IsEdited ? Vec3f::Lerp(TextColor, State->Color, 0.25f) : TextColor);

	if (NewText != TextObject->GetText())
	{
		TextObject->SetText(NewText);
		RedrawElement();
	}

	TextTimer += ParentWindow->GetDeltaTime();

	Vec2f EditedTextPos = IsEdited ? TextObject->GetLetterLocation(ParentWindow->Input.TextIndex) : 0;
	float CursorFraction = (OffsetPosition.Y - EditedTextPos.Y) / Size.Y + 1;

	if (!Dragging && IsEdited && ParentWindow->Input.TextSelectionStart == ParentWindow->Input.TextIndex)
		TextScroll.Scrolled = std::max((CursorFraction - 1) * Size.Y + 0.025f, 0.0f);
	TextScroll.Scrolled = std::min(TextScroll.Scrolled, TextScroll.MaxScroll);

	if (EditedTextPos != IBeamPosition)
	{
		float CharSize = UIText::GetTextSizeAtScale(TextObject->GetTextSize(), TextObject->GetTextFont()).Y;
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
	if (NewColor != State->Color)
	{
		State->Color = NewColor;
		ParentWindow->UI.RedrawUI();
	}
	return this;
}

Vec3f UITextField::GetColor() const
{
	return State->Color;
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

const std::string& UITextField::GetText()
{
	return EnteredText;
}

UITextField::UITextField(Vec2f Position, Vec3f Color, Font* Renderer, std::function<void()> OnChanged)
	: UIBackground(true, Position, Color)
{
	TextFieldColor = Color;
	TextObject = new UIText(11_px, Vec3f(1), HintText, Renderer);
	TextObject->SetPadding(3_px);
	TextObject->Wrap = true;
	HasMouseCollision = true;
	KeyboardFocusable = true;
	this->OnChanged = OnChanged;
	AddChild(TextObject);
}

void UITextField::Edit()
{
	IsEdited = true;
	ParentWindow->Input.PollForText = true;
	ParentWindow->Input.Text = EnteredText;
	IsPressed = false;
	ParentWindow->Input.SetTextIndex((int)EnteredText.size(), true);
	RedrawElement();
}

UITextField* kui::UITextField::SetInnerPadding(UISize Size)
{
	TextObject->SetPadding(Size);
	return this;
}

UITextField* kui::UITextField::SetInnerPadding(UISize Up, UISize Down, UISize Left, UISize Right)
{
	TextObject->SetPadding(Up, Down, Left, Right);
	return this;
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

void UITextField::DrawBackground(render::RenderBackend* Backend)
{
	TextScroll.Position = OffsetPosition;
	TextScroll.Scale = Size;

	BackgroundShader->Bind();
	TextObject->IsVisible = true;

	auto Pos = TextScroll.GetPosition();

	BackgroundShader->SetVec3("u_offset",
		Vec3f(-TextScroll.GetOffset(), Pos.Y, TextScroll.GetScale().Y));

	if (IsEdited && ParentWindow->Input.TextSelectionStart != ParentWindow->Input.TextIndex)
	{
		float CharSize = IBeamScale.Y;
		auto DrawHighlight = [Backend, this, CharSize](Vec2f Start, Vec2f End)
			{
				Vec2f BoxSize = End - Start;
				Backend->DrawSimpleBox(Start, BoxSize + Vec2f(0, CharSize), Vec3f(0.25f, 1, 1), 0);
			};

		if (TextHighlightStart.Y == TextHighlightEnd.Y)
		{
			DrawHighlight(TextHighlightStart, TextHighlightEnd);
		}
		else
		{
			std::size_t Difference = std::size_t(std::round((TextHighlightEnd.Y - TextHighlightStart.Y) / CharSize));

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
		Backend->DrawSimpleBox(IBeamPosition, IBeamScale, TextColor, 0);
	}
}