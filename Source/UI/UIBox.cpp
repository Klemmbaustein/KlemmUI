#include <UI/UIBox.h>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <Application.h>
#include <Input.h>
#include <Math/MathHelpers.h>
#include <Rendering/ScrollObject.h>
#include <UI/UIScrollBox.h>

class UIButton;

namespace UI
{
	std::set<UIBox*> ElementsToUpdate;
	std::vector<UIBox*> UIElements;
	bool RequiresRedraw = true;
	unsigned int UIBuffer = 0;
	unsigned int UITexture = 0;
}

UIBox* UIBox::SetSizeMode(E_SizeMode NewMode)
{
	if (NewMode != SizeMode)
	{
		SizeMode = NewMode;
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
	UI::UIElements.push_back(this);
}

UIBox::~UIBox()
{
	GetAbsoluteParent()->InvalidateLayout();
	DeleteChildren();
	for (unsigned int i = 0; i < UI::UIElements.size(); i++)
	{
		if (UI::UIElements[i] == this)
		{
			UI::UIElements.erase(UI::UIElements.begin() + i);
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

UIBox* UIBox::SetBorder(E_BorderType Type, double Size)
{
	if (BorderType != Type || Size != BorderRadius)
	{
		BorderType = Type;
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
	for (auto i : UI::UIElements)
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
	for (auto* elem : UI::UIElements)
	{
		if (!elem->Parent)
		{
			delete elem;
		}
	}
	UI::UIElements.clear();
	UI::RequiresRedraw = true;
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
	for (auto c : Children)
	{
		c->UpdateSelfAndChildren();
	}
	Update();
	if (Parent == nullptr)
	{
		OffsetPosition = Position;
	}
	Vector2f PreviousSize = Size;
	Vector2f AdjustedMinSize = MinSize;
	Vector2f AdjustedMaxSize = MaxSize;

	if (SizeMode == E_PIXEL_RELATIVE)
	{
		AdjustedMinSize = AdjustedMinSize / Vector2f(Application::AspectRatio, 1);
		AdjustedMaxSize = AdjustedMaxSize / Vector2f(Application::AspectRatio, 1);
	}

	double TestSize = 0.0f;
	double TestWidth = 0.0f;
	if (ChildrenHorizontal)
	{
		for (auto c : Children)
		{
			TestSize += c->Size.X + c->LeftPadding + c->RightPadding;
			if (!c->TryFill)
			{
				TestWidth += c->Size.Y + c->LeftPadding + c->RightPadding;
			}
		}
		TestSize = std::min(AdjustedMaxSize.X, TestSize);
		TestSize = std::max(AdjustedMinSize.X, TestSize);
		TestWidth = std::max(AdjustedMinSize.Y, TestWidth);
	}
	else
	{
		for (auto c : Children)
		{
			TestSize += c->Size.Y + c->LeftPadding + c->RightPadding;
			if (!c->TryFill)
			{
				TestWidth += c->Size.X + c->LeftPadding + c->RightPadding;
			}
		}
		TestSize = std::min(MaxSize.Y, TestSize);
		TestSize = std::max(AdjustedMinSize.Y, TestSize);
		TestWidth = std::max(AdjustedMinSize.X, TestWidth);
	}
	Size = Vector2f();
	for (auto c : Children)
	{
		if (ChildrenHorizontal)
		{
			if (Align == E_REVERSE)
			{
				c->OffsetPosition = Vector2((Size.X - (c->Size.X + c->RightPadding * 2) + TestSize), c->DownPadding) + OffsetPosition;
				Size.X -= c->Size.X;
				Size.X -= c->LeftPadding + c->RightPadding;
			}
			else if (Align == E_CENTERED)
			{
				c->OffsetPosition = Vector2((Size.X - (c->Size.X + c->RightPadding * 2) + TestSize), c->DownPadding) + OffsetPosition;
				Vector2 PosB = Vector2(Size.X + c->LeftPadding, c->DownPadding) + OffsetPosition;
				c->OffsetPosition = Vector2(
					std::lerp(c->OffsetPosition.X, PosB.X, 0.5),
					OffsetPosition.Y + 0.02f
				);
				Size.X -= c->Size.X;
				Size.X -= c->LeftPadding + c->RightPadding;
			}
			else
			{
				c->OffsetPosition = Vector2(Size.X + c->LeftPadding, c->DownPadding) + OffsetPosition;
				Size.X += c->Size.X;
				Size.X += c->LeftPadding + c->RightPadding;
			}
			Size.Y = std::max(c->UpPadding + c->DownPadding + c->Size.Y, Size.Y);
		}
		else
		{
			c->OffsetPosition = Vector2(c->LeftPadding, c->DownPadding + Size.Y) + OffsetPosition;

			if (Align == E_REVERSE)
			{
				c->OffsetPosition = Vector2(c->LeftPadding, (Size.Y - (c->Size.Y + c->UpPadding * 2) + TestSize)) + OffsetPosition;
				Size.Y -= c->Size.Y;
				Size.Y -= c->UpPadding + c->DownPadding;
			}
			else
			{
				c->OffsetPosition = Vector2(c->LeftPadding, Size.Y + c->DownPadding) + OffsetPosition;
				Size.Y += c->Size.Y;
				Size.Y += c->UpPadding + c->DownPadding;
			}
			Size.X = std::max(c->LeftPadding + c->RightPadding + c->Size.X, Size.X);
		}
	}
	if (Align == E_REVERSE || Align == E_CENTERED)
	{
		if (!Parent)
		{
			if (ChildrenHorizontal)
			{
				OffsetPosition = Vector2f(0, -Size.Y) + Position;
			}
			else
			{
				OffsetPosition = Position;
			}
		}
		if (ChildrenHorizontal)
		{
			Size.X = -Size.X;
			Size.Y = Size.Y;
		}
		else
		{
			Size.X = Size.X;
			Size.Y = -Size.Y;
		}
	}
	for (auto c : Children)
	{
		if (c->TryFill)
		{
			if (ChildrenHorizontal)
			{
				c->Size.Y = TestWidth - c->DownPadding - c->UpPadding;
			}
			else
			{
				c->Size.X = TestWidth - c->LeftPadding - c->RightPadding;
			}
		}
	}
	if (TryFill && Parent)
	{
		if (Parent->ChildrenHorizontal && Size.Y < PreviousSize.Y)
		{
			Size.Y = PreviousSize.Y;
		}
		if (!Parent->ChildrenHorizontal && Size.X < PreviousSize.X)
		{
			Size.X = PreviousSize.X;
		}
	}
	Size.X = std::min(AdjustedMaxSize.X, Size.X);
	Size.Y = std::min(AdjustedMaxSize.Y, Size.Y);
	Size.X = std::max(AdjustedMinSize.X, Size.X);
	Size.Y = std::max(AdjustedMinSize.Y, Size.Y);
	for (auto c : Children)
	{
		c->UpdateSelfAndChildren();
	}
	Update();
}

void UIBox::InvalidateLayout()
{
	UI::RequiresRedraw = true;
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
		std::cout << "Attached an UIObject twice" << std::endl;
		throw "oopsie";
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

bool UIBox::DrawAllUIElements()
{
	for (auto elem : UI::UIElements)
	{
		if (elem->ShouldBeTicked)
		{
			elem->Tick();
		}
	}
	if (UI::RequiresRedraw)
	{
		UI::RequiresRedraw = false;
		for (auto elem : UI::ElementsToUpdate)
		{
			elem->UpdateSelfAndChildren();
		}
		UI::ElementsToUpdate.clear();
		glViewport(0, 0, Application::GetWindowResolution().X * 2, Application::GetWindowResolution().Y * 2);
		glBindFramebuffer(GL_FRAMEBUFFER, UI::UIBuffer);
		//glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		for (auto elem : UI::UIElements)
		{
			if (elem->Parent == nullptr)
				elem->DrawThisAndChildren();
		}
		//glClearColor(0, 0, 0, 1);
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
	return (Math::IsPointIn2DBox(OffsetPosition, OffsetPosition + Size, Input::MouseLocation) // If the mouse is on top of the button
		&& (!CurrentScrollObject || // Check if we have a scroll object
			Math::IsPointIn2DBox(CurrentScrollObject->Position - CurrentScrollObject->Scale, CurrentScrollObject->Position, Input::MouseLocation))); // do some very questionable math to check if the mouse is inside the scroll area;
}

namespace UI
{
	UIButton* HoveredButton = nullptr;
	UIButton* NewHoveredButton = nullptr;
}
