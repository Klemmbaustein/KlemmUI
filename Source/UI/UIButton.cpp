#include <KlemmUI/UI/UIButton.h>
#include <GL/glew.h>
#include <KlemmUI/Rendering/Shader.h>
#include "../Rendering/VertexBuffer.h"
#include <KlemmUI/Application.h>
#include <KlemmUI/Input.h>
#include <KlemmUI/Math/MathHelpers.h>
#include <KlemmUI/Rendering/ScrollObject.h>

namespace UI
{
	extern Shader* UIShader;
}


void UIButton::Tick()
{
	if (!IsVisible)
	{
		return;
	}
	CurrentButtonState = ButtonState::Normal;
	if (UI::HoveredBox == this && !IsHovered)
	{
		RedrawUI();
		IsHovered = true;
	}
	if (IsHovered && UI::HoveredBox != this)
	{
		RedrawUI();
		IsHovered = false;
	}


	if (UI::HoveredBox == this)
	{
		CurrentButtonState = ButtonState::Hovered;
	}
	else if (IsPressed && UI::HoveredBox != this)
	{
		IsSelected = false;
		IsPressed = false;
		RedrawUI();
	}

	if (IsSelected)
	{
		CurrentButtonState = ButtonState::Hovered;
	}

	if (UI::HoveredBox == this)
	{
		if (Input::IsLMBDown)
		{
			CurrentButtonState = ButtonState::Pressed;
			if (!IsPressed)
			{
				RedrawUI();
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
				RedrawUI();
				return;
			}
			else
			{
				IsPressed = false;
				IsSelected = true;
			}
		}
	}
	else if (Input::IsLMBDown)
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
	if (PressedFunc) Application::ButtonEvents.push_back(Application::ButtonEvent(PressedFunc, nullptr, nullptr, 0));
	if (PressedFuncIndex) Application::ButtonEvents.push_back(Application::ButtonEvent(nullptr, PressedFuncIndex, nullptr, ButtonIndex));
	if (ParentOverride)
	{
		Application::ButtonEvents.push_back(Application::ButtonEvent(nullptr, nullptr, ParentOverride, ButtonIndex));
	}
}

UIButton* UIButton::SetOpacity(float NewOpacity)
{
	if (NewOpacity != Opacity)
	{
		Opacity = NewOpacity;
		RedrawUI();
	}
	return this;
}

float UIButton::GetOpacity()
{
	return Opacity;
}

void UIButton::SetCanBeDragged(bool NewCanBeDragged)
{
	CanBeDragged = NewCanBeDragged;
}

bool UIButton::GetIsSelected()
{
	return IsSelected;
}

void UIButton::SetNeedsToBeSelected(bool NeedsToBeSelected)
{
	this->NeedsToBeSelected = NeedsToBeSelected;
}

UIButton* UIButton::SetShader(Shader* NewShader)
{
	UsedShader = NewShader;
	return this;
}

bool UIButton::GetIsHovered() const
{
	return IsHovered;
}

bool UIButton::GetIsPressed() const
{
	return IsPressed;
}

UIButton* UIButton::SetUseTexture(bool UseTexture, unsigned int TextureID)
{
	this->UseTexture = UseTexture;
	this->TextureID = TextureID;
	return this;
}

UIButton* UIButton::SetColor(Vector3f32 NewColor)
{
	if (NewColor != ButtonColor)
	{
		ButtonColor = NewColor;
		RedrawUI();
	}
	return this;
}

UIButton* UIButton::SetHoveredColor(Vector3f32 NewColor)
{
	if (NewColor != HoveredColor)
	{
		HoveredColor = NewColor;
		if (IsHovered)
		{
			RedrawUI();
		}
	}
	return this;
}

UIButton* UIButton::SetPressedColor(Vector3f32 NewColor)
{
	if (NewColor != PressedColor)
	{
		PressedColor = NewColor;
		if (IsPressed)
		{
			RedrawUI();
		}
	}
	return this;
}

Vector3f32 UIButton::GetColor()
{
	return ButtonColor;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)()) : UIBackground(Horizontal, Position, Color)
{
	if (UI::UIShader == nullptr) UI::UIShader = new Shader(Application::GetShaderPath() + "/uishader.vert", Application::GetShaderPath() + "/uishader.frag");
	this->PressedFunc = PressedFunc;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;
	HasMouseCollision = true;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)(int), int ButtonIndex) : UIBackground(Horizontal, Position, Color)
{
	if (UI::UIShader == nullptr) UI::UIShader = new Shader(Application::GetShaderPath() + "/uishader.vert", Application::GetShaderPath() + "/uishader.frag");
	this->PressedFuncIndex = PressedFunc;
	this->ButtonColor = Color;
	this->HoveredColor = Color * 0.75;
	this->PressedColor = Color * 0.5;	this->ButtonIndex = ButtonIndex;
	HasMouseCollision = true;
}

UIButton::UIButton(bool Horizontal, Vector2f Position, UIButtonStyle* Style, void(*PressedFunc)()) 
	: UIButton(Horizontal, Position, 1, PressedFunc)
{
	Style->ApplyTo(this);
}

UIButton::UIButton(bool Horizontal, Vector2f Position, UIButtonStyle* Style, void(*PressedFunc)(int), int ButtonIndex)
	: UIButton(Horizontal, Position, 1, PressedFunc, ButtonIndex)
{
	Style->ApplyTo(this);

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

UIButtonStyle::UIButtonStyle(std::string Name) : UIStyle("Button: " + Name)
{
}

void UIButtonStyle::ApplyDerived(UIBox* Target)
{
	UIButton* TargetButton = ToSafeElemPtr<UIButton>(Target);
	TargetButton->SetColor(Color);
	TargetButton->SetHoveredColor(HoveredColor);
	TargetButton->SetPressedColor(PressedColor);
	TargetButton->SetUseTexture(UseTexture, TextureID);
	TargetButton->SetOpacity(Opacity);
}
