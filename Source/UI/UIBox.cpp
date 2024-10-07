#include <kui/UI/UIBox.h>
#include <vector>
#include <GL/glew.h>
#include <kui/App.h>
#include <kui/Input.h>
#include "../Internal/MathHelpers.h"
#include <kui/Rendering/ScrollObject.h>
#include <kui/UI/UIScrollBox.h>
#include <cmath>
#include <kui/Window.h>
#include <iostream>

using namespace kui;

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

void UIBox::GetPadding(Vec2f& UpDown, Vec2f& LeftRight) const
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

UIBox::UIBox(bool Horizontal, Vec2f Position)
{
	this->Position = Position;
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

Vec2f UIBox::GetUsedSize()
{
	return Size;
}

Vec2f UIBox::GetScreenPosition() const
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

UIBox* UIBox::SetMaxSize(Vec2f NewMaxSize)
{
	if (NewMaxSize != MaxSize)
	{
		MaxSize = NewMaxSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

Vec2f UIBox::GetMaxSize() const
{
	return MaxSize;
}

UIBox* UIBox::SetMinSize(Vec2f NewMinSize)
{
	if (NewMinSize != MinSize)
	{
		MinSize = NewMinSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

Vec2f UIBox::GetMinSize() const
{
	return MinSize;
}

UIBox* UIBox::SetPosition(Vec2f NewPosition)
{
	if (NewPosition != Position)
	{
		Position = NewPosition;
		InvalidateLayout();
	}
	return this;
}

Vec2f UIBox::GetPosition() const
{
	if (CurrentScrollObject)
	{
		return OffsetPosition + Vec2f(0, CurrentScrollObject->Percentage);
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

Vec2f UIBox::GetLeftRightPadding(const UIBox* Target) const
{
	if (Target->PaddingSizeMode != SizeMode::AspectRelative)
	{
		return Vec2f(Target->LeftPadding, Target->RightPadding);
	}
	return Vec2f(Target->LeftPadding, Target->RightPadding) / (float)ParentWindow->GetAspectRatio();
}

Vec2f UIBox::PixelSizeToScreenSize(Vec2f PixelSize, Window* TargetWindow)
{
	PixelSize.X = PixelSize.X / (float)TargetWindow->GetSize().X * 2 * TargetWindow->GetDPI();
	PixelSize.Y = PixelSize.Y / (float)TargetWindow->GetSize().Y * 2 * TargetWindow->GetDPI();
	return PixelSize;
}

void kui::UIBox::SetOffsetPosition(Vec2f NewPos)
{
	if (NewPos != OffsetPosition)
	{
		Vec2f ScreenPos = GetPosition();
		OffsetPosition = NewPos;
		Vec2f NewScreenPos = GetPosition();
		ParentWindow->UI.RedrawArea(UIManager::RedrawBox{
			.Min = Vec2f::Min(ScreenPos, NewScreenPos),
			.Max = Vec2f::Max(ScreenPos, NewScreenPos) + GetUsedSize(),
			});
	}
}

UIManager::RedrawBox UIBox::GetRedrawBox() const
{
	return UIManager::RedrawBox{
		.Min = OffsetPosition,
		.Max = OffsetPosition + Size,
	};
}

float UIBox::GetVerticalOffset()
{
	Vec2f UpDown, LeftRight;
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
	Vec2f UpDown, LeftRight;
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
	Vec2f NewSize = 0;
	for (auto c : Children)
	{
		Vec2f UpDown, LeftRight;
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
		Vec2f UpDown, LeftRight;
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

	Vec2f AdjustedMinSize = MinSize;
	Vec2f AdjustedMaxSize = MaxSize;
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
			.Max = GetPosition() + Vec2f::Max(Size, NewSize),
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
			Vec2f UpDown, LeftRight;
			c->GetPaddingScreenSize(UpDown, LeftRight);
			ChildrenSize += ChildrenHorizontal ? (c->Size.X + LeftRight.X + LeftRight.Y) : (c->Size.Y + UpDown.X + UpDown.Y);
		}
	}


	for (auto c : Children)
	{
		Vec2f UpDown, LeftRight;
		c->GetPaddingScreenSize(UpDown, LeftRight);
		if (PrimaryAlign == Align::Centered)
		{
			if (ChildrenHorizontal)
			{
				c->SetOffsetPosition(OffsetPosition + Vec2f(Size.X / 2 - ChildrenSize / 2 + LeftRight.X + Offset, c->GetVerticalOffset()));
				Offset += c->Size.X + LeftRight.X + LeftRight.Y;
			}
			else
			{
				c->SetOffsetPosition(OffsetPosition + Vec2f(c->GetHorizontalOffset(), Size.Y / 2 - ChildrenSize / 2 + UpDown.Y + Offset));
				Offset += c->Size.Y + UpDown.X + UpDown.Y;
			}
		}
		else
		{
			if (ChildrenHorizontal)
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->SetOffsetPosition(OffsetPosition + Vec2f(Size.X - Offset - c->Size.X - LeftRight.Y, c->GetVerticalOffset()));
				}
				else
				{
					c->SetOffsetPosition(OffsetPosition + Vec2f(Offset + LeftRight.X, c->GetVerticalOffset()));
				}
				Offset += c->Size.X + LeftRight.X + LeftRight.Y;
			}
			else
			{
				if (PrimaryAlign == Align::Reverse)
				{
					c->SetOffsetPosition(OffsetPosition + Vec2f(c->GetHorizontalOffset(), Size.Y - Offset - c->Size.Y - UpDown.X));
				}
				else
				{
					c->SetOffsetPosition(OffsetPosition + Vec2f(c->GetHorizontalOffset(), Offset + UpDown.Y));
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

void UIBox::GetPaddingScreenSize(Vec2f& UpDown, Vec2f& LeftRight) const
{
	UpDown.X = UpPadding;
	UpDown.Y = DownPadding;

	if (PaddingSizeMode == SizeMode::PixelRelative)
	{
		UpDown = UpDown / (float)ParentWindow->GetSize().Y * ParentWindow->GetDPI() * 2;
	}

	LeftRight = GetLeftRightPadding(this);
	if (PaddingSizeMode == SizeMode::PixelRelative)
	{
		LeftRight = LeftRight / (float)ParentWindow->GetSize().X * ParentWindow->GetDPI() * 2;
	}
}

void UIBox::RedrawElement(bool Force)
{
	if ((!IsVisibleInHierarchy() || Redrawn) && !Force)
	{
		return;
	}

	Redrawn = true;
	ParentWindow->UI.RedrawArea(GetRedrawBox());
}

void kui::UIBox::SetUpPadding(float Value)
{
	if (UpPadding != Value)
	{
		UpPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetDownPadding(float Value)
{
	if (DownPadding != Value)
	{
		DownPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetLeftPadding(float Value)
{
	if (LeftPadding != Value)
	{
		LeftPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetRightPadding(float Value)
{
	if (RightPadding != Value)
	{
		RightPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
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
		app::error::Error("Attached an UIObject twice", true);
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
void UIBox::UpdateHoveredState()
{
	if (IsBeingHovered() && HasMouseCollision && IsVisibleInHierarchy())
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
	LastDrawIndex++;
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
	Redrawn = false;
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
	Vec2f MouseLocation = ParentWindow->Input.MousePosition;
	if (CurrentScrollObject)
	{
		MouseLocation = MouseLocation - Vec2f(0, CurrentScrollObject->Percentage);
	}
	return (internal::math::IsPointIn2DBox(OffsetPosition, OffsetPosition + Size, MouseLocation) // If the mouse is on top of the box
		&& (!CurrentScrollObject || // Check if we have a scroll object
			// If there is a scroll object, is the mouse on top of the box with it's scroll offset
			internal::math::IsPointIn2DBox(CurrentScrollObject->Position - CurrentScrollObject->Scale,
				CurrentScrollObject->Position, ParentWindow->Input.MousePosition)));
}
