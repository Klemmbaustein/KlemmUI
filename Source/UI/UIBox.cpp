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
	GetAbsoluteParent()->InvalidateLayout();
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
		for (int i = 0; i < Parent->Children.size(); i++)
		{
			if (Parent->Children[i] == this)
			{
				Parent->Children.erase(Parent->Children.begin() + i);
			}
		}
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

Vector2f UIBox::GetLeftRightPadding(UIBox* Target)
{
	if (Target->PaddingSizeMode == SizeMode::ScreenRelative)
	{
		return Vector2f(Target->LeftPadding, Target->RightPadding);
	}
	return Vector2f(Target->LeftPadding, Target->RightPadding) / (float)ParentWindow->GetAspectRatio();
}

Vector2f UIBox::PixelSizeToScreenSize(Vector2f PixelSize, Window* TargetWindow)
{
	PixelSize.X = PixelSize.X / (float)TargetWindow->GetSize().X * 1080;
	PixelSize.Y = PixelSize.Y / (float)TargetWindow->GetSize().Y * 1080;
	return PixelSize;
}

float UIBox::GetVerticalOffset()
{
	float VerticalOffset = DownPadding;

	if (Parent->VerticalBoxAlign == Align::Reverse)
	{
		VerticalOffset = Parent->Size.Y - UpPadding - Size.Y;
	}
	else if (Parent->VerticalBoxAlign == Align::Centered)
	{
		VerticalOffset = std::lerp(Parent->Size.Y - UpPadding - Size.Y, DownPadding, 0.5f);
	}
	return VerticalOffset;
}

float UIBox::GetHorizontalOffset()
{
	float HorizontalOffset = GetLeftRightPadding(this).X;

	if (Parent->HorizontalBoxAlign == Align::Reverse)
	{
		HorizontalOffset = Parent->Size.X - GetLeftRightPadding(this).Y - Size.X;
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
	Size = 0;
	for (auto c : Children)
	{
		if (ChildrenHorizontal)
		{
			Size.X += c->Size.X + GetLeftRightPadding(c).X + GetLeftRightPadding(c).Y;
			if (!c->TryFill)
			{
				Size.Y = std::max(Size.Y, c->Size.Y + c->UpPadding + c->DownPadding);
			}
		}
		else
		{
			Size.Y += c->Size.Y + c->UpPadding + c->DownPadding;
			if (!c->TryFill)
			{
				Size.X = std::max(Size.X, c->Size.X + GetLeftRightPadding(c).X + GetLeftRightPadding(c).Y);
			}
		}
	}

	if (TryFill && Parent)
	{
		if (Parent->ChildrenHorizontal)
		{
			Size.Y = Parent->Size.Y - (UpPadding + DownPadding);
		}
		else
		{
			Size.X = Parent->Size.X - (GetLeftRightPadding(this).X + GetLeftRightPadding(this).Y);
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

	Size = Size.Clamp(AdjustedMinSize, AdjustedMaxSize);
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
		OffsetPosition = Position;
	}

	Align PrimaryAlign = ChildrenHorizontal ? HorizontalBoxAlign : VerticalBoxAlign;

	float ChildrenSize = 0;

	if (PrimaryAlign == Align::Centered)
	{
		for (auto c : Children)
		{
			Vector2 LeftRight = GetLeftRightPadding(c);
			ChildrenSize += ChildrenHorizontal ? (c->Size.X + LeftRight.X + LeftRight.Y) : (c->Size.Y + c->UpPadding + c->DownPadding);
		}
	}


	for (auto c : Children)
	{
		if (PrimaryAlign == Align::Centered)
		{
			if (ChildrenHorizontal)
			{
				c->OffsetPosition = OffsetPosition + Vector2f(Size.X / 2 - ChildrenSize / 2 + GetLeftRightPadding(c).X + Offset, c->GetVerticalOffset());
				Offset += c->Size.X + GetLeftRightPadding(c).X + GetLeftRightPadding(c).Y;
			}
			else
			{
				c->OffsetPosition = OffsetPosition + Vector2f(c->GetHorizontalOffset(), Size.Y / 2 - ChildrenSize / 2 + c->DownPadding + Offset);
				Offset += c->Size.Y + c->DownPadding + c->UpPadding;
			}
		}
		else
		{
			if (ChildrenHorizontal)
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->OffsetPosition = OffsetPosition + Vector2f(Size.X - Offset - c->Size.X - GetLeftRightPadding(c).Y, c->GetVerticalOffset());
				}
				else
				{
					c->OffsetPosition = OffsetPosition + Vector2f(Offset + GetLeftRightPadding(c).X, c->GetVerticalOffset());
				}
				Offset += c->Size.X + GetLeftRightPadding(c).X + GetLeftRightPadding(c).Y;
			}
			else
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->OffsetPosition = OffsetPosition + Vector2f(c->GetHorizontalOffset(), Size.Y - Offset - c->Size.Y - c->UpPadding);
				}
				else
				{
					c->OffsetPosition = OffsetPosition + Vector2f(c->GetHorizontalOffset(), Offset + c->DownPadding);
				}
				Offset += c->Size.Y + c->DownPadding + c->UpPadding;
			}
		}
	}
	for (auto c : Children)
	{
		c->UpdatePosition();
		c->Update();
	}
}

void UIBox::InvalidateLayout()
{
	ParentWindow->UI.RedrawUI();

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
		GetAbsoluteParent()->InvalidateLayout();
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

void UIBox::DrawThisAndChildren()
{
	for (auto c : Children)
	{
		c->UpdateTickState();
	}
	if (IsVisible)
	{
		Draw();
		for (auto c : Children)
		{
			c->DrawThisAndChildren();
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
