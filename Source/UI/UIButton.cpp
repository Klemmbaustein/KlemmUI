#include <kui/UI/UIButton.h>
#include <kui/Window.h>
#include <kui/UI/UIScrollBox.h>
#include "../Rendering/VertexBuffer.h"
#include <iostream>
using namespace kui;

void UIButton::Tick()
{
	if (!IsVisible)
	{
		return;
	}

	if (!ParentWindow->HasFocus())
	{
		return;
	}

	bool Hovered = ParentWindow->UI.HoveredBox == this && !UIScrollBox::IsDraggingScrollBox;

	CurrentButtonState = ButtonState::Normal;
	if (Hovered && !this->IsHovered)
	{
		RedrawElement();
		this->IsHovered = true;
	}
	if (IsHovered && ParentWindow->UI.HoveredBox != this)
	{
		RedrawElement();
		this->IsHovered = false;
	}


	if (Hovered)
	{
		CurrentButtonState = ButtonState::Hovered;
	}
	else if (IsPressed && ParentWindow->UI.HoveredBox != this && !IsKeyboardFocused)
	{
		IsSelected = false;
		if (OnDragged)
		{
			ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(
				nullptr, OnDragged, ButtonIndex));
		}
		IsPressed = false;
		RedrawElement();
	}

	if (IsSelected)
	{
		CurrentButtonState = ButtonState::Hovered;
	}

	if (ParentWindow->UI.KeyboardFocusBox == this)
	{
		if (!IsKeyboardFocused)
		{
			BorderRadius *= 2;
			RedrawElement();
		}
		CurrentButtonState = ButtonState::KeyboardHovered;
		IsKeyboardFocused = true;
	}
	else if (IsKeyboardFocused)
	{
		RedrawElement();
		IsKeyboardFocused = false;
		BorderRadius /= 2;
	}

	if (Hovered || IsKeyboardFocused)
	{
		if (ParentWindow->Input.IsLMBDown || ParentWindow->Input.IsKeyDown(Key::RETURN))
		{
			CurrentButtonState = ButtonState::Pressed;
			if (!IsPressed)
			{
				RedrawElement();
				IsPressed = true;
			}
		}
		else if (IsPressed)
		{
			if (!NeedsToBeSelected || IsSelected)
			{
				OnButtonClicked();
				IsPressed = false;
				IsSelected = false;
				RedrawElement();
			}
			else
			{
				IsPressed = false;
				IsSelected = true;
			}
		}
	}
	else if (ParentWindow->Input.IsLMBDown)
	{
		IsSelected = false;
	}

	switch (CurrentButtonState)
	{
	case UIButton::ButtonState::Normal:
		UIBackground::Color = ButtonColor;
		break;
	case UIButton::ButtonState::Hovered:
		UIBackground::Color = HoveredColor;
		break;
	case UIButton::ButtonState::KeyboardHovered:
		UIBackground::Color = KeyboardHoveredColor;
		break;
	case UIButton::ButtonState::Pressed:
		UIBackground::Color = PressedColor;
		break;
	default:
		break;
	}

}

void UIButton::OnButtonClicked()
{
	if (OnClicked)
		ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(
			OnClicked, nullptr, 0));
	if (OnClickedIndex)
		ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(
			nullptr, OnClickedIndex, ButtonIndex));
}

bool UIButton::GetIsSelected() const
{
	return IsSelected;
}

void UIButton::SetNeedsToBeSelected(bool NeedsToBeSelected)
{
	this->NeedsToBeSelected = NeedsToBeSelected;
}

bool UIButton::GetIsHovered() const
{
	return IsHovered;
}

bool UIButton::GetIsPressed() const
{
	return IsPressed;
}

UIBackground* UIButton::SetColor(Vec3f NewColor)
{
	if (NewColor != ButtonColor)
	{
		ButtonColor = NewColor;
		if (!IsHovered && !IsPressed)
		{
			Color = ButtonColor;
		}
		RedrawElement();
	}
	return this;
}

UIButton* UIButton::SetHoveredColor(Vec3f NewColor)
{
	if (NewColor != HoveredColor)
	{
		HoveredColor = NewColor;
		if (IsHovered)
		{
			Color = ButtonColor;
			RedrawElement();
		}
	}
	return this;
}

UIButton* UIButton::SetKeyboardHoveredColor(Vec3f NewColor)
{
	if (NewColor != KeyboardHoveredColor)
	{
		KeyboardHoveredColor = NewColor;
		if (IsHovered)
		{
			Color = KeyboardHoveredColor;
			RedrawElement();
		}
	}
	return this;
}

UIButton* UIButton::SetPressedColor(Vec3f NewColor)
{
	if (NewColor != PressedColor)
	{
		PressedColor = NewColor;
		if (IsPressed)
		{
			Color = NewColor;
			RedrawElement();
		}
	}
	return this;
}

Vec3f UIButton::GetColor() const
{
	return ButtonColor;
}

UIButton::UIButton(bool Horizontal, Vec2f Position, Vec3f Color, std::function<void()> OnClickedFunction) 
	: UIBackground(Horizontal, Position, Color)
{
	this->OnClicked = OnClickedFunction;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->KeyboardHoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;
	HasMouseCollision = true;
	KeyboardFocusable = true;
}

UIButton::UIButton(bool Horizontal, Vec2f Position, Vec3f Color,std::function<void(int)> OnClickedFunction, int ButtonIndex)
	: UIBackground(Horizontal, Position, Color)
{
	this->OnClickedIndex = OnClickedFunction;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;
	this->ButtonIndex = ButtonIndex;
	HasMouseCollision = true;
	KeyboardFocusable = true;
}

UIButton::~UIButton()
{
}

void UIButton::Update()
{
}

void UIButton::DrawBackground()
{
}
