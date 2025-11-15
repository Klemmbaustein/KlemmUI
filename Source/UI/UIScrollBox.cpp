#include <kui/UI/UIScrollBox.h>
#include <kui/UI/UIButton.h>
#include <kui/UI/UIBackground.h>
#include <kui/Window.h>
#include <utility>

using namespace kui;

bool UIScrollBox::IsDraggingScrollBox = false;
Vec3f UIScrollBox::BackgroundColor = 0.25f;
Vec3f UIScrollBox::ScrollBarColor = 0.75f;
Vec3f UIScrollBox::BackgroundBorderColor = 0.15f;

float UIScrollBox::GetDesiredChildrenSize()
{
	float DesiredSize = 0;
	if (!ChildrenHorizontal)
	{
		for (UIBox* i : Children)
		{
			if (i->IsCollapsed)
				continue;
			Vec2f UpDown;
			Vec2f LeftRight;
			i->GetPaddingScreenSize(UpDown, LeftRight);
			DesiredSize += UpDown.X + UpDown.Y + std::max(i->GetUsedSize().GetScreen().Y, 0.0f);
		}
	}
	else
	{
		for (UIBox* i : Children)
		{
			if (i->IsCollapsed)
				continue;
			Vec2f UpDown;
			Vec2f LeftRight;
			i->GetPaddingScreenSize(UpDown, LeftRight);
			DesiredSize = std::max(UpDown.X + UpDown.Y + std::max(i->GetUsedSize().GetScreen().Y, 0.0f), DesiredSize);
		}
	}
	return DesiredSize;
}

void UIScrollBox::UpdateScrollObjectOfObject(UIBox* o)
{
	if (o != this)
	{
		o->CurrentScrollObject = &ScrollClass;
		if (dynamic_cast<UIScrollBox*>(o) && o != this)
		{
			return;
		}
	}

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
			ScrollBarBackground = new UIButton(false, 0, BackgroundColor, nullptr, 0);
			ScrollBarBackground
				->SetHoveredColor(BackgroundColor)
				->SetPressedColor(BackgroundColor);
			ScrollBarBackground->SetBorder(1_px, BackgroundBorderColor);
			ScrollBarBackground->SetPosition(OffsetPosition + Vec2f(Size.X - ScrollBarBackground->GetUsedSize().GetScreen().Y, 0));
			ScrollBar = new UIBackground(true, 0, ScrollBarColor);
			ScrollBarBackground->AddChild(ScrollBar);
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
	ScrollClass.Active = ParentWindow->UI.HoveredBox && (ParentWindow->UI.HoveredBox == this
		|| ParentWindow->UI.HoveredBox->IsChildOf(this));
	ScrollClass.Parent = CurrentScrollObject;
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
		ScrollBarBackground->SetMinSize(SizeVec(UISize::Smallest(), GetUsedSize().Y));
		ScrollBarBackground->SetPosition(OffsetPosition
			+ Vec2f(Size.X - ScrollBarBackground->GetUsedSize().GetScreen().X, 0));

		float ScrollPercentage = ScrollClass.Scrolled / ScrollClass.MaxScroll;

		Vec2f Pixel = PixelSizeToScreenSize(1, ParentWindow);
		float Height = Size.Y - ScrollDownPadding * Pixel.Y;

		if (DesiredMaxScroll <= Height)
		{
			ScrollBar->SetMinSize(SizeVec(Vec2f((ScrollBarWidth - 4)
				* Pixel.X, Height - 4 * Pixel.Y), SizeMode::ScreenRelative));
			ScrollBar->SetPadding(UISize::Aspect(0.0025f));
			ScrollPercentage = 0;
		}
		else
		{
			ScrollBar->SetMinSize(SizeVec(Vec2f((ScrollBarWidth - 4) * Pixel.X,
				std::max(Size.Y / (DesiredMaxScroll / Height), (30_px).GetScreen().Y)), SizeMode::ScreenRelative));

			ScrollBar->SetPadding(UISize::Screen(std::max((ScrollPercentage * Height)
				- (ScrollPercentage * ScrollBar->GetMinSize().GetScreen().Y) - 4 * Pixel.Y, 2 * Pixel.Y)),
				UISize::Pixels(2),
				UISize::Pixels(2),
				UISize::Pixels(2));
		}
		if (((ScrollBarBackground->GetIsPressed() && !IsDraggingScrollBox) || IsDragging) && ScrollClass.MaxScroll)
		{
			if (ParentWindow->Input.MousePosition == 99)
			{
				return;
			}

			if (!IsDragging && ScrollBar->IsBeingHovered())
			{
				InitialScrollPosition = ScrollClass.Scrolled;
				InitialDragPosition = ParentWindow->Input.MousePosition.Y;
			}
			else if (IsDragging)
			{
				float Fraction = (ParentWindow->Input.MousePosition.Y - InitialDragPosition)
					/ (Height - ScrollBar->GetUsedSize().GetScreen().Y);

				float NewPercentage = std::max(
					std::min(
						InitialScrollPosition - Fraction * ScrollClass.MaxScroll,
						ScrollClass.MaxScroll),
					0.0f);

				if (NewPercentage != ScrollClass.Scrolled)
				{
					ScrollClass.Scrolled = NewPercentage;
					RedrawElement();
				}
			}
			else
			{
				InitialScrollPosition = ScrollClass.Scrolled;
				InitialDragPosition = ScrollBar->GetPosition().Y + ScrollBar->GetUsedSize().GetScreen().Y / 2;
			}
			IsDragging = true;
			IsDraggingScrollBox = true;
		}
	}
	if (!ParentWindow->Input.IsLMBDown)
	{
		IsDraggingScrollBox = false;
		IsDragging = false;
	}

	if (OldPercentage != ScrollClass.Scrolled)
	{
		OldPercentage = ScrollClass.Scrolled;
		if (OnScroll)
		{
			OnScroll(this);
		}
	}
}

void UIScrollBox::SetMaxScroll(float NewMaxScroll)
{
	MaxScroll = NewMaxScroll;
	Update();
}

float UIScrollBox::GetMaxScroll() const
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
	float Progress = ScrollClass.Scrolled;
	float Speed = ScrollClass.Speed;
	float ActualMaxScroll = MaxScroll;
	DesiredMaxScroll = MaxScroll + Size.Y;
	if (MaxScroll == -1)
	{
		DesiredMaxScroll = GetDesiredChildrenSize();
		ActualMaxScroll = std::max(DesiredMaxScroll - Size.Y, 0.0f);
	}
	ScrollClass = ScrollObject(OffsetPosition, Size, ActualMaxScroll);
	ScrollClass.Scrolled = Progress;
	ScrollClass.Speed = Speed;
	if (ScrollBar && UseDefaultColors)
	{
		ScrollBarBackground
			->SetHoveredColor(BackgroundColor)
			->SetPressedColor(BackgroundColor)
			->SetColor(BackgroundColor);
		ScrollBarBackground->SetBorder(1_px, BackgroundBorderColor);
		ScrollBar->SetColor(ScrollBarColor);
	}
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


UIScrollBox::UIScrollBox(bool Horizontal, Vec2f Position, bool DisplayScrollBar) : UIBox(Horizontal, Position)
{
	HasMouseCollision = true;
	SetDisplayScrollBar(DisplayScrollBar);
	Update();
}

UIScrollBox::~UIScrollBox()
{
	SetDisplayScrollBar(false);
}
