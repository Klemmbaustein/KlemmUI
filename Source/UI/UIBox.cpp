#include <kui/UI/UIBox.h>
#include <vector>
#include <GL/glew.h>
#include <kui/App.h>
#include <kui/Input.h>
#include "../Internal/MathHelpers.h"
#include <kui/UI/UIScrollBox.h>
#include <cmath>
#include <kui/Window.h>
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

void UIBox::GetPadding(SizeVec& UpDown, SizeVec& LeftRight) const
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
	if (ParentWindow->UI.KeyboardFocusBox == this)
	{
		ParentWindow->UI.KeyboardFocusBox = nullptr;
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

SizeVec UIBox::GetUsedSize()
{
	return SizeVec(Size, SizeMode::ScreenRelative);
}

Vec2f UIBox::GetScreenPosition() const
{
	if (CurrentScrollObject)
		return OffsetPosition + Vec2f(0, CurrentScrollObject->GetOffset());
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

UIBox* UIBox::SetMaxSize(SizeVec NewMaxSize)
{
	if (NewMaxSize != MaxSize)
	{
		MaxSize = NewMaxSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

SizeVec UIBox::GetMaxSize() const
{
	return MaxSize;
}

UIBox* UIBox::SetMinSize(SizeVec NewMinSize)
{
	if (NewMinSize != MinSize)
	{
		MinSize = NewMinSize;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}


SizeVec UIBox::GetMinSize() const
{
	return MinSize;
}

UIBox* kui::UIBox::SetMinWidth(UISize NewWidth)
{
	if (NewWidth != MinSize.X)
	{
		MinSize.X = NewWidth;
		InvalidateLayout();
	}
	return this;
}

UIBox* kui::UIBox::SetMinHeight(UISize NewHeight)
{
	if (NewHeight != MinSize.Y)
	{
		MinSize.Y = NewHeight;
		InvalidateLayout();
	}
	return this;
}

UIBox* kui::UIBox::SetMaxWidth(UISize NewWidth)
{
	if (NewWidth != MaxSize.X)
	{
		MaxSize.X = NewWidth;
		InvalidateLayout();
	}
	return this;
}

UIBox* kui::UIBox::SetMaxHeight(UISize NewHeight)
{
	if (NewHeight != MaxSize.Y)
	{
		MaxSize.Y = NewHeight;
		InvalidateLayout();
	}
	return this;
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
		return OffsetPosition + Vec2f(0, CurrentScrollObject->GetOffset());
	}
	else
	{
		return OffsetPosition;
	}
}

UIBox* UIBox::SetPadding(UISize Up, UISize Down, UISize Left, UISize Right)
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

UIBox* UIBox::SetPadding(UISize AllDirs)
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

void UIBox::Update()
{
}

void UIBox::UpdateSelfAndChildren()
{
	UpdateScale();
	UpdatePosition();

	Update();
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
			.Max = Vec2f::Max(ScreenPos, NewScreenPos) + GetUsedSize().GetScreen(),
			});
	}
}

UIManager::RedrawBox UIBox::GetRedrawBox() const
{
	return UIManager::RedrawBox{
		.Min = GetScreenPosition(),
		.Max = GetScreenPosition() + Size,
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
			if (c->MinSize.X.Mode != SizeMode::ParentRelative)
			{
				NewSize.X += c->Size.X + LeftRight.X + LeftRight.Y;
			}
			if (c->MinSize.Y.Mode != SizeMode::ParentRelative)
			{
				NewSize.Y = std::max(NewSize.Y, c->Size.Y + UpDown.X + UpDown.Y);
			}
		}
		else
		{
			if (c->MinSize.Y.Mode != SizeMode::ParentRelative)
			{
				NewSize.Y += c->Size.Y + UpDown.X + UpDown.Y;
			}
			if (c->MinSize.X.Mode != SizeMode::ParentRelative)
			{
				NewSize.X = std::max(NewSize.X, c->Size.X + LeftRight.X + LeftRight.Y);
			}
		}
	}

	Vec2f ScreenMinSize = MinSize.GetScreen();
	Vec2f ScreenMaxSize = MaxSize.GetScreen();

	Vec2f UpDown, LeftRight;
	GetPaddingScreenSize(UpDown, LeftRight);
	Vec2f AvailableParentSize = (Parent ? Parent->Size : Vec2f(2)) - Vec2f(LeftRight.X + LeftRight.Y, UpDown.X + UpDown.Y);

	if (MinSize.X.Mode == SizeMode::ParentRelative)
	{
		ScreenMinSize.X = AvailableParentSize.X * MinSize.X.Value;
	}
	if (MinSize.Y.Mode == SizeMode::ParentRelative)
	{
		ScreenMinSize.Y = AvailableParentSize.Y * MinSize.Y.Value;
	}
	if (MaxSize.X.Mode == SizeMode::ParentRelative)
	{
		ScreenMaxSize.X = AvailableParentSize.X * MaxSize.X.Value;
	}
	if (MaxSize.Y.Mode == SizeMode::ParentRelative)
	{
		ScreenMaxSize.Y = AvailableParentSize.Y * MaxSize.Y.Value;
	}

	NewSize = NewSize.Clamp(ScreenMinSize, ScreenMaxSize);

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
	UpDown.X = UpPadding.GetScreen().Y;
	UpDown.Y = DownPadding.GetScreen().Y;

	LeftRight.X = LeftPadding.GetScreen().X;
	LeftRight.Y = RightPadding.GetScreen().X;
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

void kui::UIBox::SetUpPadding(UISize Value)
{
	if (UpPadding != Value)
	{
		UpPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetDownPadding(UISize Value)
{
	if (DownPadding != Value)
	{
		DownPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetLeftPadding(UISize Value)
{
	if (LeftPadding != Value)
	{
		LeftPadding = Value;
		GetAbsoluteParent()->InvalidateLayout();
	}
}

void kui::UIBox::SetRightPadding(UISize Value)
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
	if (NewChild == this)
	{
		app::error::Error("Attached an UIBox to itself", true);
	}

	if (!NewChild->Parent)
	{
		NewChild->Parent = this;
		Children.push_back(NewChild);
		NewChild->OnAttached();
		InvalidateLayout();
	}
	else
	{
		app::error::Error("Attached an UIBox twice", true);
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
		MouseLocation = MouseLocation - Vec2f(0, CurrentScrollObject->GetOffset());
	}
	return (internal::math::IsPointIn2DBox(OffsetPosition, OffsetPosition + Size, MouseLocation) // If the mouse is on top of the box
		&& (!CurrentScrollObject || // Check if we have a scroll object
			// If there is a scroll object, is the mouse on top of the box with it's scroll offset
			internal::math::IsPointIn2DBox(CurrentScrollObject->Position + CurrentScrollObject->Scale,
				CurrentScrollObject->Position, ParentWindow->Input.MousePosition)));
}
