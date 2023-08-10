#include <UI/UIScrollBox.h>
#include <Math/MathHelpers.h>
#include <Input.h>
#include <UI/UIBox.h>
#include <iostream>

namespace _ScrollObject
{
	static std::set<ScrollObject*> AllScrollObjects;
}

std::set<ScrollObject*> ScrollObject::GetAllScrollObjects()
{
	return _ScrollObject::AllScrollObjects;
}

ScrollObject::ScrollObject(Vector2f Position, Vector2f Scale, float MaxScroll)
{
	this->Position = Position;
	this->Scale = Vector2f() - Scale;
	_ScrollObject::AllScrollObjects.insert(this);
	this->MaxScroll = MaxScroll;
}

ScrollObject::~ScrollObject()
{
	_ScrollObject::AllScrollObjects.erase(this);
}

void ScrollObject::ScrollUp()
{
	if (!Active)
	{
		return;
	}
	if (Math::IsPointIn2DBox(Position - Scale, Position, Input::MouseLocation))
	{
		Percentage += Speed / 100.f;
	}
	if (Percentage > MaxScroll)
	{
		Percentage = MaxScroll;
	}
	UIBox::RedrawUI();
}

void ScrollObject::ScrollDown()
{
	if (!Active)
	{
		return;
	}
	if (Math::IsPointIn2DBox(Position - Scale, Position, Input::MouseLocation))
	{
		Percentage -= Speed / 100.f;
	}
	if (Percentage < 0)
		Percentage = 0;
	UIBox::RedrawUI();
}
