#include <UI/UIScrollBox.h>
#include <iostream>

void UIScrollBox::UpdateScrollObjectOfObject(UIBox* o)
{
	o->CurrentScrollObject = &ScrollClass;
	for (auto c : o->Children)
	{
		UpdateScrollObjectOfObject(c);
	}
}

ScrollObject* UIScrollBox::GetScrollObject()
{
	return &ScrollClass;
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

void UIScrollBox::Update()
{
	float Progress = ScrollClass.Percentage;
	ScrollClass = ScrollObject(OffsetPosition, Size, MaxScroll);
	ScrollClass.Percentage = Progress;
	UpdateScrollObjectOfObject(this);
	ScrollClass.IsActive = IsVisibleInHierarchy();
}

void UIScrollBox::Tick()
{
	ScrollClass.IsActive = IsVisibleInHierarchy();
}


UIScrollBox::UIScrollBox(bool Horizontal, Vector2f Position, float MaxScroll) : UIBox(Horizontal, Position)
{
	this->MaxScroll = MaxScroll;
	Update();
}
