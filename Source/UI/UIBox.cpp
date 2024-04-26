#include <KlemmUI/UI/UIBox.h>
#include <vector>
#include <GL/glew.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Input.h>
#include "../MathHelpers.h"
#include <KlemmUI/Rendering/ScrollObject.h>
#include <KlemmUI/UI/UIScrollBox.h>
#include <cmath>
#include <KlemmUI/Window.h>
#include <iostream>

using namespace KlemmUI;

bool UIBox::IsChildOf(UIBox* Parent)
{
	if (Parent == this->Parent)
	{
		return true;
	}
	if (!this->Parent)
	{
		return false;
	}
	return this->Parent->IsChildOf(Parent);
}

void UIBox::GetPadding(Vector2f& UpDown, Vector2f& LeftRight) const
{
	UpDown.X = UpPadding;
	UpDown.Y = DownPadding;
	LeftRight.X = LeftPadding;
	LeftRight.Y = RightPadding;
}

const std::vector<UIBox*>& UIBox::GetChildren()
{
	return Children;
}

void UIBox::UpdateElement()
{
	UpdateSelfAndChildren();
}

UIBox* UIBox::SetSizeMode(SizeMode NewMode)
{
	if (NewMode != BoxSizeMode)
	{
		BoxSizeMode = NewMode;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

UIBox::UIBox(bool Horizontal, Vector2f Position)
{
	this->Position = Position;
	this->Size = Size;
	this->ChildrenHorizontal = Horizontal;
	ParentWindow = Window::GetActiveWindow();
	ParentWindow->UI.UIElements.push_back(this);
}

UIBox::~UIBox()
{
	InvalidateLayout();
	DeleteChildren();
	if (ParentWindow->UI.HoveredBox == this)
	{
		ParentWindow->UI.HoveredBox = nullptr;
	}
	if (ParentWindow->UI.NewHoveredBox == this)
	{
		ParentWindow->UI.NewHoveredBox = nullptr;
	}

	for (unsigned int i = 0; i < ParentWindow->UI.UIElements.size(); i++)
	{
		if (ParentWindow->UI.UIElements[i] == this)
		{
			ParentWindow->UI.UIElements.erase(ParentWindow->UI.UIElements.begin() + i);
		}
	}
	ParentWindow->UI.ElementsToUpdate.erase(this);
	if (Parent)
	{
		Parent->RedrawElement();
		for (int i = 0; i < Parent->Children.size(); i++)
		{
			if (Parent->Children[i] == this)
			{
				Parent->Children.erase(Parent->Children.begin() + i);
			}
		}
	}
	else
	{
		RedrawElement();
	}
}

void UIBox::Draw()
{
}

void UIBox::Tick()
{
}

void UIBox::UpdateTickState()
{
	for (auto c : Children)
	{
		c->UpdateTickState();
	}
	ShouldBeTicked = IsVisibleInHierarchy();
}

UIBox* UIBox::GetParent()
{
	return Parent;
}

Window* UIBox::GetParentWindow()
{
	return ParentWindow;
}

void UIBox::OnChildClicked(int Index)
{
}

UIBox* UIBox::SetBorder(BorderType Type, float Size)
{
	if (BoxBorder != Type || Size != BorderRadius)
	{
		BoxBorder = Type;
		BorderRadius = Size;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

void UIBox::MoveToFront()
{
	for (size_t i = 0; i < ParentWindow->UI.UIElements.size(); i++)
	{
		if (ParentWindow->UI.UIElements[i] == this)
		{
			ParentWindow->UI.UIElements.erase(ParentWindow->UI.UIElements.begin() + i);
			break;
		}
	}
	ParentWindow->UI.UIElements.push_back(this);
}

Vector2f UIBox::GetUsedSize()
{
	return Size;
}

Vector2f UIBox::GetScreenPosition() const
{
	return OffsetPosition;
}

void UIBox::SetCurrentScrollObject(UIScrollBox* s)
{
	SetCurrentScrollObject(s->GetScrollObject());
}

void UIBox::SetCurrentScrollObject(ScrollObject* s)
{
	CurrentScrollObject = s;
	for (auto& c : Children)
	{
		c->SetCurrentScrollObject(s);
	}
}

void UIBox::OnAttached()
{
}

UIBox* UIBox::SetMaxSize(Vector2f NewMaxSize)
{
	if (NewMaxSize != MaxSize)
	{
		MaxSize = NewMaxSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

Vector2f UIBox::GetMaxSize() const
{
	return MaxSize;
}

UIBox* UIBox::SetMinSize(Vector2f NewMinSize)
{
	if (NewMinSize != MinSize)
	{
		MinSize = NewMinSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

Vector2f UIBox::GetMinSize() const
{
	return MinSize;
}

UIBox* UIBox::SetPosition(Vector2f NewPosition)
{
	if (NewPosition != Position)
	{
		Position = NewPosition;
		InvalidateLayout();
		ParentWindow->UI.RedrawUI();
	}
	return this;
}

Vector2f UIBox::GetPosition()
{
	if (CurrentScrollObject)
	{
		return OffsetPosition + Vector2f(0, CurrentScrollObject->Percentage);
	}
	else
	{
		return OffsetPosition;
	}
}

UIBox* UIBox::SetPadding(float Up, float Down, float Left, float Right)
{
	if (Up != UpPadding || Down != DownPadding || Left != LeftPadding || Right != RightPadding)
	{
		UpPadding = Up;
		DownPadding = Down;
		LeftPadding = Left;
		RightPadding = Right;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

UIBox* UIBox::SetPadding(float AllDirs)
{
	if (AllDirs != UpPadding || AllDirs != DownPadding || AllDirs != LeftPadding || AllDirs != RightPadding)
	{
		UpPadding = AllDirs;
		DownPadding = AllDirs;
		LeftPadding = AllDirs;
		RightPadding = AllDirs;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

UIBox* UIBox::SetPaddingSizeMode(SizeMode NewSizeMode)
{
	if (NewSizeMode != PaddingSizeMode)
	{
		PaddingSizeMode = NewSizeMode;
		InvalidateLayout();
	}
	return this;
}

UIBox* UIBox::SetTryFill(bool NewTryFill)
{
	if (TryFill != NewTryFill)
	{
		TryFill = NewTryFill;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

UIBox* UIBox::SetHorizontalAlign(Align NewAlign)
{
	HorizontalBoxAlign = NewAlign;
	return this;
}

UIBox* UIBox::SetVerticalAlign(Align NewAlign)
{
	VerticalBoxAlign = NewAlign;
	return this;
}

UIBox* UIBox::SetHorizontal(bool IsHorizontal)
{
	if (IsHorizontal != ChildrenHorizontal)
	{
		ChildrenHorizontal = IsHorizontal;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

bool UIBox::GetTryFill() const
{
	return TryFill;
}

void UIBox::Update()
{
}

void UIBox::UpdateSelfAndChildren()
{
	UpdateScale();
	UpdatePosition();

	Update();
}

Vector2f UIBox::GetLeftRightPadding(const UIBox* Target) const
{
	if (Target->PaddingSizeMode != SizeMode::AspectRelative)
	{
		return Vector2f(Target->LeftPadding, Target->RightPadding);
	}
	return Vector2f(Target->LeftPadding, Target->RightPadding) / (float)ParentWindow->GetAspectRatio();
}

Vector2f UIBox::PixelSizeToScreenSize(Vector2f PixelSize, Window* TargetWindow)
{
	PixelSize.X = (PixelSize.X) / (float)TargetWindow->GetSize().X * 2 * TargetWindow->GetDPI();
	PixelSize.Y = (PixelSize.Y) / (float)TargetWindow->GetSize().Y * 2 * TargetWindow->GetDPI();
	return PixelSize;
}

void KlemmUI::UIBox::SetOffsetPosition(Vector2f NewPos)
{
	if (NewPos != OffsetPosition)
	{
		Vector2f ScreenPos = GetPosition();
		OffsetPosition = NewPos;
		Vector2f NewScreenPos = GetPosition();
		ParentWindow->UI.RedrawArea(UIManager::RedrawBox{
			.Min = Vector2f::Min(ScreenPos, NewScreenPos),
			.Max = Vector2f::Max(ScreenPos, NewScreenPos) + GetUsedSize(),
			});
	}
}

float UIBox::GetVerticalOffset()
{
	Vector2f UpDown, LeftRight;
	GetPaddingScreenSize(UpDown, LeftRight);

	float VerticalOffset = UpDown.Y;

	if (Parent->VerticalBoxAlign == Align::Reverse)
	{
		VerticalOffset = Parent->Size.Y - UpDown.X - Size.Y;
	}
	else if (Parent->VerticalBoxAlign == Align::Centered)
	{
		VerticalOffset = std::lerp(Parent->Size.Y - UpDown.X - Size.Y, UpDown.Y, 0.5f);
	}
	return VerticalOffset;
}

float UIBox::GetHorizontalOffset()
{
	Vector2f UpDown, LeftRight;
	GetPaddingScreenSize(UpDown, LeftRight);
	float HorizontalOffset = LeftRight.X;

	if (Parent->HorizontalBoxAlign == Align::Reverse)
	{
		HorizontalOffset = Parent->Size.X - LeftRight.Y - Size.X;
	}
	else if (Parent->HorizontalBoxAlign == Align::Centered)
	{
		HorizontalOffset = Parent->Size.X / 2 - Size.X / 2;
	}
	return HorizontalOffset;
}

void UIBox::UpdateScale()
{
	for (auto c : Children)
	{
		c->UpdateScale();
	}
	Vector2f NewSize = 0;
	for (auto c : Children)
	{
		Vector2f UpDown, LeftRight;
		c->GetPaddingScreenSize(UpDown, LeftRight);

		if (ChildrenHorizontal)
		{
			NewSize.X += c->Size.X + LeftRight.X + LeftRight.Y;
			if (!c->TryFill)
			{
				NewSize.Y = std::max(NewSize.Y, c->Size.Y + UpDown.X + UpDown.Y);
			}
		}
		else
		{
			NewSize.Y += c->Size.Y + UpDown.X + UpDown.Y;
			if (!c->TryFill)
			{
				NewSize.X = std::max(NewSize.X, c->Size.X + LeftRight.X + LeftRight.Y);
			}
		}
	}

	if (TryFill && Parent)
	{
		Vector2f UpDown, LeftRight;
		GetPaddingScreenSize(UpDown, LeftRight);
		if (Parent->ChildrenHorizontal)
		{
			NewSize.Y = Parent->Size.Y - (UpDown.X + UpDown.Y);
			MinSize.Y = 0;
			MaxSize.Y = 999999;
		}
		else
		{
			NewSize.X = Parent->Size.X - (LeftRight.X + LeftRight.Y);
			MinSize.X = 0;
			MaxSize.X = 999999;
		}
	}

	Vector2f AdjustedMinSize = MinSize;
	Vector2f AdjustedMaxSize = MaxSize;
	if (BoxSizeMode == SizeMode::AspectRelative)
	{
		AdjustedMinSize.X /= ParentWindow->GetAspectRatio();
		AdjustedMaxSize.X /= ParentWindow->GetAspectRatio();
	}
	if (BoxSizeMode == SizeMode::PixelRelative)
	{
		AdjustedMinSize = PixelSizeToScreenSize(AdjustedMinSize, ParentWindow);
		AdjustedMaxSize = PixelSizeToScreenSize(AdjustedMaxSize, ParentWindow);
	}
	NewSize = NewSize.Clamp(AdjustedMinSize, AdjustedMaxSize);

	if (NewSize != Size)
	{
		ParentWindow->UI.RedrawArea(UIManager::RedrawBox{
			.Min = GetPosition(),
			.Max = GetPosition() + Vector2f::Max(Size, NewSize),
			});
		Size = NewSize;
	}
	for (auto c : Children)
	{
		c->UpdateScale();
	}
}

void UIBox::UpdatePosition()
{
	float Offset = 0;

	if (!Parent)
	{
		SetOffsetPosition(Position);
	}

	Align PrimaryAlign = ChildrenHorizontal ? HorizontalBoxAlign : VerticalBoxAlign;

	float ChildrenSize = 0;

	if (PrimaryAlign == Align::Centered)
	{
		for (auto c : Children)
		{
			Vector2f UpDown, LeftRight;
			c->GetPaddingScreenSize(UpDown, LeftRight);
			ChildrenSize += ChildrenHorizontal ? (c->Size.X + LeftRight.X + LeftRight.Y) : (c->Size.Y + UpDown.X + UpDown.Y);
		}
	}


	for (auto c : Children)
	{
		Vector2f UpDown, LeftRight;
		c->GetPaddingScreenSize(UpDown, LeftRight);
		if (PrimaryAlign == Align::Centered)
		{
			if (ChildrenHorizontal)
			{
				c->SetOffsetPosition(OffsetPosition + Vector2f(Size.X / 2 - ChildrenSize / 2 + LeftRight.X + Offset, c->GetVerticalOffset()));
				Offset += c->Size.X + LeftRight.X + LeftRight.Y;
			}
			else
			{
				c->SetOffsetPosition(OffsetPosition + Vector2f(c->GetHorizontalOffset(), Size.Y / 2 - ChildrenSize / 2 + UpDown.Y + Offset));
				Offset += c->Size.Y + UpDown.X + UpDown.Y;
			}
		}
		else
		{
			if (ChildrenHorizontal)
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->SetOffsetPosition(OffsetPosition + Vector2f(Size.X - Offset - c->Size.X - LeftRight.Y, c->GetVerticalOffset()));
				}
				else
				{
					c->SetOffsetPosition(OffsetPosition + Vector2f(Offset + LeftRight.X, c->GetVerticalOffset()));
				}
				Offset += c->Size.X + LeftRight.X + LeftRight.Y;
			}
			else
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->SetOffsetPosition(OffsetPosition + Vector2f(c->GetHorizontalOffset(), Size.Y - Offset - c->Size.Y - UpDown.X));
				}
				else
				{
					c->SetOffsetPosition(OffsetPosition + Vector2f(c->GetHorizontalOffset(), Offset + UpDown.Y));
				}
				Offset += c->Size.Y + UpDown.X + UpDown.Y;
			}
		}
	}
	for (auto c : Children)
	{
		c->UpdatePosition();
		c->Update();
	}
}

void UIBox::GetPaddingScreenSize(Vector2f& UpDown, Vector2f& LeftRight) const
{
	UpDown.X = UpPadding;
	UpDown.Y = DownPadding;

	if (PaddingSizeMode == SizeMode::PixelRelative)
	{
		UpDown = UpDown / (float)ParentWindow->GetSize().Y * 2;
	}

	LeftRight = GetLeftRightPadding(this);
	if (PaddingSizeMode == SizeMode::PixelRelative)
	{
		LeftRight = LeftRight / (float)ParentWindow->GetSize().X * 2;
	}
}

void UIBox::RedrawElement(bool Force)
{
	if (!IsVisibleInHierarchy() && !Force)
	{
		return;
	}

	ParentWindow->UI.RedrawArea(UIManager::RedrawBox{
		.Min = GetPosition(),
		.Max = GetPosition() + GetUsedSize(),
		});
}

void UIBox::InvalidateLayout()
{
	if (Parent)
	{
		Parent->InvalidateLayout();
	}
	else
	{
		ParentWindow->UI.ElementsToUpdate.insert(this);
	}
}

UIBox* UIBox::AddChild(UIBox* NewChild)
{
	if (!NewChild->Parent)
	{
		NewChild->Parent = this;
		Children.push_back(NewChild);
		NewChild->OnAttached();
		InvalidateLayout();
	}
	else
	{
		Application::Error::Error("Attached an UIObject twice");
		throw 0;
	}
	return this;
}

UIBox* UIBox::GetAbsoluteParent()
{
	if (Parent != nullptr)
	{
		return Parent->GetAbsoluteParent();
	}
	return this;
}

bool UIBox::IsHovered()
{
	Vector2f Offset;
	if (CurrentScrollObject)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	return Math::IsPointIn2DBox(OffsetPosition + Offset, OffsetPosition + Size + Offset, Window::GetActiveWindow()->Input.MousePosition) // If the mouse is on top of the box
		&& (!CurrentScrollObject // Check if we have a scroll object
			|| Math::IsPointIn2DBox( // do some very questionable math to check if the mouse is inside the scroll area
				CurrentScrollObject->Position - CurrentScrollObject->Scale,
				CurrentScrollObject->Position, Window::GetActiveWindow()->Input.MousePosition));
}

void UIBox::UpdateHoveredState()
{
	if (IsHovered() && HasMouseCollision && IsVisibleInHierarchy())
	{
		ParentWindow->UI.NewHoveredBox = this;
	}
	for (UIBox* Child : Children)
	{
		Child->UpdateHoveredState();
	}
}

void UIBox::DrawThisAndChildren(const UIManager::RedrawBox& Box)
{
	for (auto c : Children)
	{
		c->UpdateTickState();
	}
	if (IsVisible)
	{
		if (UIManager::RedrawBox::IsBoxOverlapping(Box, UIManager::RedrawBox{
			.Min = GetPosition(),
			.Max = GetPosition() + Size
			}))
		{
			Draw();
		}
		for (auto c : Children)
		{
			c->DrawThisAndChildren(Box);
		}
	}
}

void UIBox::DeleteChildren()
{
	while (Children.size() != 0)
	{
		delete Children[0];
	}
	Children.clear();
}

bool UIBox::IsVisibleInHierarchy()
{
	if (!Parent) return IsVisible;
	if (IsVisible) return Parent->IsVisibleInHierarchy();
	return false;
}

bool UIBox::IsBeingHovered()
{
	Vector2f MouseLocation = ParentWindow->Input.MousePosition;
	if (CurrentScrollObject)
	{
		MouseLocation = MouseLocation - Vector2f(0, CurrentScrollObject->Percentage);
	}
	return (Math::IsPointIn2DBox(OffsetPosition, OffsetPosition + Size, MouseLocation) // If the mouse is on top of the button
		&& (!CurrentScrollObject || // Check if we have a scroll object
			Math::IsPointIn2DBox(CurrentScrollObject->Position - CurrentScrollObject->Scale,
				CurrentScrollObject->Position, ParentWindow->Input.MousePosition))); // do some very questionable math to check if the mouse is inside the scroll area;
}
