#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/UI/UIButton.h>
#include <KlemmUI/UI/UIBackground.h>
#include <KlemmUI/Window.h>
#include <cmath>
#include <KlemmUI/Application.h>

using namespace KlemmUI;

bool UIScrollBox::IsDraggingScrollBox = false;

float UIScrollBox::GetDesiredChildrenSize()
{
	float DesiredSize = 0;
	for (UIBox* i : Children)
	{
		Vector2f UpDown;
		Vector2f LeftRight;
		i->GetPaddingScreenSize(UpDown, LeftRight);
		DesiredSize += UpDown.X + UpDown.Y + std::max(i->GetUsedSize().Y, 0.0f);
	}
	return DesiredSize;
}

void UIScrollBox::UpdateScrollObjectOfObject(UIBox* o)
{
	o->CurrentScrollObject = &ScrollClass;
	for (auto c : o->GetChildren())
	{
		UpdateScrollObjectOfObject(c);
	}
}

ScrollObject* UIScrollBox::GetScrollObject()
{
	return &ScrollClass;
}

UIBackground* UIScrollBox::GetScrollBarSlider()
{
	return ScrollBar;
}

UIButton* UIScrollBox::GetScrollBarBackground()
{
	return ScrollBarBackground;
}

UIScrollBox* UIScrollBox::SetDisplayScrollBar(bool NewDisplay)
{
	if (NewDisplay != DisplayScrollBar)
	{
		DisplayScrollBar = NewDisplay;
		if (DisplayScrollBar)
		{
			ScrollBarBackground = new UIButton(false, 0, 0.3f, nullptr, 0);
			ScrollBarBackground->ParentOverride = this;
			ScrollBarBackground->SetBorder(UIBox::BorderType::DarkenedEdge, 1.0f);
			ScrollBarBackground->SetBorderColor(0.1f);
			ScrollBarBackground->SetBorderSizeMode(UIBox::SizeMode::PixelRelative);
			ScrollBarBackground->SetVerticalAlign(UIBox::Align::Reverse);
			ScrollBarBackground->SetPosition(OffsetPosition + Vector2f(Size.X - ScrollBarBackground->GetUsedSize().X, 0));
			ScrollBar = new UIBackground(true, 0, 0.75, Vector2f(0.01f, 0.1f));
			ScrollBarBackground->AddChild(ScrollBar);
			ScrollBar->SetBorder(UIBox::BorderType::Rounded, 0.25);
			ScrollBar->SetPadding(0);
		}
		else if (ScrollBar)
		{
			delete ScrollBarBackground;
			ScrollBarBackground = nullptr;
			ScrollBar = nullptr;
		}
	}
	return this;
}

bool UIScrollBox::GetDisplayScrollBar() const
{
	return DisplayScrollBar;
}

void UIScrollBox::Tick()
{
	ScrollClass.Active = ParentWindow->UI.HoveredBox && (ParentWindow->UI.HoveredBox == this || ParentWindow->UI.HoveredBox->IsChildOf(this));
	CurrentScrollObject = nullptr;
	bool VisibleInHierarchy = IsVisibleInHierarchy();
	if (MaxScroll == -1)
	{
		DesiredMaxScroll = GetDesiredChildrenSize();
	}
	else
	{
		DesiredMaxScroll = MaxScroll + Size.Y;
	}
	if (ScrollBarBackground)
	{
		ScrollBarBackground->IsVisible = VisibleInHierarchy && DesiredMaxScroll > Size.Y;
	}
	if (ScrollBar && VisibleInHierarchy)
	{
		ScrollBarBackground->SetMinSize(Vector2f(0, GetUsedSize().Y));
		ScrollBarBackground->SetPosition(OffsetPosition + Vector2f(Size.X - ScrollBarBackground->GetUsedSize().X, 0));

		float ScrollPercentage = ScrollClass.Percentage / ScrollClass.MaxScroll;

		if (DesiredMaxScroll <= Size.Y)
		{
			ScrollBar->SetMinSize(Vector2f(0.01f, Size.Y - 0.005f));
			ScrollBar->SetPadding(0.0025f);
			ScrollPercentage = 0;
		}
		else
		{
			ScrollBar->SetMinSize(Vector2f(0.01f, Size.Y / (DesiredMaxScroll / Size.Y)));

			ScrollBar->SetPadding(std::max((ScrollPercentage * Size.Y) - (ScrollPercentage * ScrollBar->GetUsedSize().Y) - 0.005f, 0.0025f),
				0.0025f,
				0.0025f,
				0.0025f);
		}
		if ((ScrollBarBackground->GetIsPressed() && !IsDraggingScrollBox) || IsDragging && ScrollClass.MaxScroll)
		{
			if (!IsDragging && ScrollBar->IsBeingHovered())
			{
				InitialScrollPosition = ScrollClass.Percentage;
				InitialDragPosition = Window::GetActiveWindow()->Input.MousePosition.Y;
			}
			else if (IsDragging)
			{
				float NewPercentage = std::max(
					std::min(
						InitialScrollPosition - (float)((Window::GetActiveWindow()->Input.MousePosition.Y - InitialDragPosition) * Size.Y / ScrollBar->GetUsedSize().Y),
						ScrollClass.MaxScroll),
					0.0f);

				if (NewPercentage != ScrollClass.Percentage)
				{
					ScrollClass.Percentage = NewPercentage;
					RedrawElement();
				}
			}
			IsDragging = true;
			IsDraggingScrollBox = true;
		}
	}
	if (!Window::GetActiveWindow()->Input.IsLMBDown)
	{
		IsDraggingScrollBox = false;
		IsDragging = false;
	}

	if (OldPercentage != ScrollClass.Percentage)
	{
		OldPercentage = ScrollClass.Percentage;
		if (OnScrolled)
		{
			OnScrolled(this);
		}
	}
}

void UIScrollBox::SetMaxScroll(float NewMaxScroll)
{
	MaxScroll = NewMaxScroll;
	Update();
}

float UIScrollBox::GetMaxScroll()
{
	return MaxScroll;
}

UIScrollBox* UIScrollBox::SetScrollSpeed(float NewScrollSpeed)
{
	ScrollClass.Speed = NewScrollSpeed;
	return this;
}

float UIScrollBox::GetScrollSpeed() const
{
	return ScrollClass.Speed;
}

void UIScrollBox::Update()
{
	float Progress = ScrollClass.Percentage;
	float Speed = ScrollClass.Speed;
	float ActualMaxScroll = MaxScroll;
	DesiredMaxScroll = MaxScroll + Size.Y;
	if (MaxScroll == -1)
	{
		DesiredMaxScroll = GetDesiredChildrenSize();
		ActualMaxScroll = std::max(DesiredMaxScroll - Size.Y, 0.0f);
	}
	ScrollClass = ScrollObject(OffsetPosition, Size, ActualMaxScroll);
	ScrollClass.Percentage = Progress;
	ScrollClass.Speed = Speed;
	UpdateScrollObjectOfObject(this);

}

void UIScrollBox::UpdateTickState()
{
	for (auto c : Children)
	{
		c->UpdateTickState();
	}
	ShouldBeTicked = true;
}


UIScrollBox::UIScrollBox(bool Horizontal, Vector2f Position, bool DisplayScrollBar) : UIBox(Horizontal, Position)
{
	this->MaxScroll = MaxScroll;
	this->HasMouseCollision = true;
	SetDisplayScrollBar(DisplayScrollBar);
	Update();
}

UIScrollBox::~UIScrollBox()
{
	SetDisplayScrollBar(false);
}
