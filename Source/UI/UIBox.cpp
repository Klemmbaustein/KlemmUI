#include <UI/UIBox.h>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <Application.h>
#include <Input.h>
#include <UI/UIStyle.h>
#include <Math/MathHelpers.h>
#include <Rendering/ScrollObject.h>
#include <UI/UIScrollBox.h>
#include <Math/MathHelpers.h>

namespace UI
{
	std::set<UIBox*> ElementsToUpdate;
	bool RequiresRedraw = true;
	unsigned int UIBuffer = 0;
	unsigned int UITexture = 0;
}
std::vector<UIBox*> UIBox::UIElements;

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
	GetAbsoluteParent()->InvalidateLayout();
	UIElements.push_back(this);
}

UIBox::UIBox(UIStyle* UsedStyle, bool Horizontal, Vector2f Position)
{
	this->Position = Position;
	this->ChildrenHorizontal = Horizontal;
	UsedStyle->ApplyTo(this);
	GetAbsoluteParent()->InvalidateLayout();
	UIElements.push_back(this);
}

UIBox::~UIBox()
{
	GetAbsoluteParent()->InvalidateLayout();
	DeleteChildren();
	if (UI::HoveredBox == this)
	{
		UI::HoveredBox = nullptr;
	}
	if (UI::NewHoveredBox == this)
	{
		UI::NewHoveredBox = nullptr;
	}

	for (unsigned int i = 0; i < UIElements.size(); i++)
	{
		if (UIElements[i] == this)
		{
			UIElements.erase(UIElements.begin() + i);
		}
	}
	UI::ElementsToUpdate.erase(this);
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

void UIBox::OnChildClicked(int Index)
{
}

UIBox* UIBox::SetBorder(BorderType Type, double Size)
{
	if (BoxBorder != Type || Size != BorderRadius)
	{
		BoxBorder = Type;
		BorderRadius = Size;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

void UIBox::ForceUpdateUI()
{
	if (UI::UIBuffer)
	{
		glDeleteFramebuffers(1, &UI::UIBuffer);
		glDeleteTextures(1, &UI::UITexture);
	}
	UI::UIBuffer = 0;
	UI::UITexture = 0;
	InitUI();
	for (UIBox* i : UIElements)
	{
		if (!i->Parent)
		{
			i->InvalidateLayout();
		}
	}
}

void UIBox::InitUI()
{
	glGenFramebuffers(1, &UI::UIBuffer);
	// create floating point color buffer
	glGenTextures(1, &UI::UITexture);
	glBindTexture(GL_TEXTURE_2D, UI::UITexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
		Application::GetWindowResolution().X * 2, Application::GetWindowResolution().Y * 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, UI::UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UI::UITexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int UIBox::GetUIFramebuffer()
{
	return UI::UITexture;
}


void UIBox::RedrawUI()
{
	UI::RequiresRedraw = true;
}

void UIBox::ClearUI()
{
	UI::ElementsToUpdate.clear();
	for (UIBox* elem : UIElements)
	{
		if (!elem->Parent)
		{
			delete elem;
		}
	}
	UIElements.clear();
	RedrawUI();
}

bool UIBox::GetShouldRedrawUI()
{
	return UI::RequiresRedraw;
}

Vector2f UIBox::GetUsedSize()
{
	return Size;
}

Vector2f UIBox::GetScreenPosition()
{
	return OffsetPosition;
}

void UIBox::SetCurrentScrollObject(UIScrollBox* s)
{
	CurrentScrollObject = s->GetScrollObject();
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

Vector2f UIBox::GetMaxSize()
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

Vector2f UIBox::GetMinSize()
{
	return MinSize;
}

UIBox* UIBox::SetPosition(Vector2f NewPosition)
{
	if (NewPosition != Position)
	{
		Position = NewPosition;
		UpdateSelfAndChildren();
		RedrawUI();
	}
	return this;
}

Vector2f UIBox::GetPosition()
{
	return OffsetPosition + Vector2f(0, CurrentScrollObject->Percentage);
}

UIBox* UIBox::SetPadding(double Up, double Down, double Left, double Right)
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

UIBox* UIBox::SetPadding(double AllDirs)
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

UIBox* UIBox::SetTryFill(bool NewTryFill)
{
	if (TryFill != NewTryFill)
	{
		TryFill = NewTryFill;
		GetAbsoluteParent()->InvalidateLayout();
	}
	return this;
}

UIBox* UIBox::SetAlign(Align NewAlign)
{
	BoxAlign = NewAlign;
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

bool UIBox::GetTryFill()
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

Vector2f UIBox::PixelSizeToScreenSize(Vector2f PixelSize)
{
	PixelSize.X = PixelSize.X / (double)Application::GetWindowResolution().X * 1080;
	PixelSize.Y = PixelSize.Y / (double)Application::GetWindowResolution().Y * 1080;
	return PixelSize;
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
			Size.X += c->Size.X + c->LeftPadding + c->RightPadding;
			Size.Y = std::max(Size.Y, c->Size.Y + c->UpPadding + c->DownPadding);
		}
		else
		{
			Size.Y += c->Size.Y + c->UpPadding + c->DownPadding;
			Size.X = std::max(Size.X, c->Size.X + c->LeftPadding + c->RightPadding);
		}
	}

	Vector2f AdjustedMinSize = MinSize;
	Vector2f AdjustedMaxSize = MaxSize;
	if (BoxSizeMode == SizeMode::AspectRelative)
	{
		AdjustedMinSize.X /= Application::AspectRatio;
		AdjustedMaxSize.X /= Application::AspectRatio;
	}
	if (BoxSizeMode == SizeMode::PixelRelative)
	{
		AdjustedMinSize = PixelSizeToScreenSize(AdjustedMinSize);
		AdjustedMaxSize = PixelSizeToScreenSize(AdjustedMaxSize);
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

	float ChildrenSize = 0;

	if (BoxAlign == Align::Centered)
	{
		for (auto c : Children)
		{
			ChildrenSize += ChildrenHorizontal ? (c->Size.X + c->LeftPadding + c->RightPadding) : (c->Size.Y + c->UpPadding + c->DownPadding);
		}
	}


	for (auto c : Children)
	{
		if (BoxAlign == Align::Centered)
		{
			if (ChildrenHorizontal)
			{
				c->OffsetPosition = OffsetPosition + Vector2f(Size.X / 2 - ChildrenSize / 2 + c->LeftPadding, c->DownPadding);
				Offset += c->Size.X + c->LeftPadding + c->RightPadding;
			}
			else
			{
				c->OffsetPosition = OffsetPosition + Vector2f(c->LeftPadding, Size.Y / 2 - ChildrenSize / 2 + c->DownPadding);
				Offset += c->Size.Y + c->DownPadding + c->UpPadding;
			}
		}
		else
		{
			if (ChildrenHorizontal)
			{
				if (BoxAlign == Align::Reverse)
				{
					c->OffsetPosition = OffsetPosition + Vector2f(Size.X - Offset - c->Size.X - c->RightPadding, c->DownPadding);
				}
				else
				{
					c->OffsetPosition = OffsetPosition + Vector2f(Offset + c->LeftPadding, c->DownPadding);
				}
				Offset += c->Size.X + c->LeftPadding + c->RightPadding;
			}
			else
			{
				if (BoxAlign == Align::Reverse)
				{
					c->OffsetPosition = OffsetPosition + Vector2f(c->LeftPadding, Size.Y - Offset - c->Size.Y - c->UpPadding);
				}
				else
				{
					c->OffsetPosition = OffsetPosition + Vector2f(c->LeftPadding, Offset + c->DownPadding);
				}
				Offset += c->Size.Y + c->DownPadding + c->UpPadding;
			}
		}
	}
	for (auto c : Children)
	{
		c->UpdatePosition();
		c->Update();
		if (c->TryFill)
		{
			if (ChildrenHorizontal)
			{
				c->Size.Y = Size.Y - (c->UpPadding + c->DownPadding);
				c->Size = c->Size.Clamp(c->MinSize, c->MaxSize);
			}
			else
			{
				c->Size.X = Size.X - (c->LeftPadding + c->RightPadding);
				c->Size = c->Size.Clamp(c->MinSize, c->MaxSize);
			}
		}
	}
}

void UIBox::InvalidateLayout()
{
	RedrawUI();
	UI::ElementsToUpdate.insert(this);
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
		Application::Error("Attached an UIObject twice");
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
	return Math::IsPointIn2DBox(OffsetPosition + Offset, OffsetPosition + Size + Offset, Input::MouseLocation) // If the mouse is on top of the box
		&& (!CurrentScrollObject // Check if we have a scroll object
			|| Math::IsPointIn2DBox( // do some very questionable math to check if the mouse is inside the scroll area
				CurrentScrollObject->Position - CurrentScrollObject->Scale,
				CurrentScrollObject->Position, Input::MouseLocation));
}

void UIBox::UpdateHoveredState()
{
	if (IsHovered() && HasMouseCollision && IsVisibleInHierarchy())
	{
		UI::NewHoveredBox = this;
	}
	for (UIBox* Child : Children)
	{
		Child->UpdateHoveredState();
	}
}

bool UIBox::DrawAllUIElements()
{
	UI::NewHoveredBox = nullptr;

	for (UIBox* elem : UIElements)
	{
		if (elem->IsVisible != elem->PrevIsVisible)
		{
			RedrawUI();
			elem->PrevIsVisible = elem->IsVisible;
		}
		if (elem->ShouldBeTicked)
		{
			elem->Tick();
		}
		if (!elem->Parent)
		{
			elem->UpdateHoveredState();
		}
	}
	UI::HoveredBox = UI::NewHoveredBox;

	if (UI::RequiresRedraw)
	{
		UI::RequiresRedraw = false;
		for (UIBox* elem : UI::ElementsToUpdate)
		{
			elem->UpdateSelfAndChildren();
		}
		UI::ElementsToUpdate.clear();
		glViewport(0, 0, Application::GetWindowResolution().X * 2, Application::GetWindowResolution().Y * 2);
		glBindFramebuffer(GL_FRAMEBUFFER, UI::UIBuffer);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		for (UIBox* elem : UIElements)
		{
			if (elem->Parent == nullptr)
				elem->DrawThisAndChildren();
		}
		glViewport(0, 0, Application::GetWindowResolution().X, Application::GetWindowResolution().Y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}
	return false;
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
	Vector2f MouseLocation = Input::MouseLocation;
	if (CurrentScrollObject)
	{
		MouseLocation = MouseLocation - Vector2f(0, CurrentScrollObject->Percentage);
	}
	return (Math::IsPointIn2DBox(OffsetPosition, OffsetPosition + Size, MouseLocation) // If the mouse is on top of the button
		&& (!CurrentScrollObject || // Check if we have a scroll object
			Math::IsPointIn2DBox(CurrentScrollObject->Position - CurrentScrollObject->Scale,
				CurrentScrollObject->Position, Input::MouseLocation))); // do some very questionable math to check if the mouse is inside the scroll area;
}

namespace UI
{
	UIBox* HoveredBox = nullptr;
	UIBox* NewHoveredBox = nullptr;
}
