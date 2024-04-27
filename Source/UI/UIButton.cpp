#include <KlemmUI/UI/UIButton.h>
#include <GL/glew.h>
#include <KlemmUI/Rendering/Shader.h>
#include "../Rendering/VertexBuffer.h"
#include <KlemmUI/Application.h>
#include <KlemmUI/Window.h>
#include <KlemmUI/Rendering/ScrollObject.h>

using namespace KlemmUI;

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

	CurrentButtonState = ButtonState::Normal;
	if (ParentWindow->UI.HoveredBox == this && !IsHovered)
	{
		RedrawElement();
		IsHovered = true;
	}
	if (IsHovered && ParentWindow->UI.HoveredBox != this)
	{
		RedrawElement();
		IsHovered = false;
	}


	if (ParentWindow->UI.HoveredBox == this)
	{
		CurrentButtonState = ButtonState::Hovered;
	}
	else if (IsPressed && ParentWindow->UI.HoveredBox != this)
	{
		IsSelected = false;
		if (OnDragged)
		{
			ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(nullptr, OnDragged, nullptr, ButtonIndex));
		}
		IsPressed = false;
		RedrawElement();
	}

	if (IsSelected)
	{
		CurrentButtonState = ButtonState::Hovered;
	}

	if (ParentWindow->UI.HoveredBox == this)
	{
		if (ParentWindow->Input.IsLMBDown)
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
				OnClicked();
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
	case UIButton::ButtonState::Pressed:
		UIBackground::Color = PressedColor;
		break;
	default:
		break;
	}

}

void UIButton::OnClicked()
{
	if (PressedFunc) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(PressedFunc, nullptr, nullptr, 0));
	if (PressedFuncIndex) ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(nullptr, PressedFuncIndex, nullptr, ButtonIndex));
	if (ParentOverride)
	{
		ParentWindow->UI.ButtonEvents.push_back(UIManager::ButtonEvent(nullptr, nullptr, ParentOverride, ButtonIndex));
	}
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

UIBackground* UIButton::SetColor(Vector3f NewColor)
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

UIButton* UIButton::SetHoveredColor(Vector3f NewColor)
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

UIButton* UIButton::SetPressedColor(Vector3f NewColor)
{
	if (NewColor != PressedColor)
	{
		PressedColor = NewColor;
		if (IsPressed)
		{
			Color = ButtonColor;
			RedrawElement();
		}
	}
	return this;
}

Vector3f UIButton::GetColor()
{
	return ButtonColor;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)()) : UIBackground(Horizontal, Position, Color)
{
	this->PressedFunc = PressedFunc;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;
	HasMouseCollision = true;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)(int), int ButtonIndex) : UIBackground(Horizontal, Position, Color)
{
	this->PressedFuncIndex = PressedFunc;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;	this->ButtonIndex = ButtonIndex;
	HasMouseCollision = true;
}

UIButton::~UIButton()
{
	delete ButtonVertexBuffer;
}

void UIButton::Update()
{
}

void UIButton::DrawBackground()
{
}
