#include <KlemmUI/UI/UIScrollBox.h>
#include "../MathHelpers.h"
#include <KlemmUI/UI/UIBox.h>
#include <KlemmUI/Window.h>

using namespace KlemmUI;

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
	if (Math::IsPointIn2DBox(Position - Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Percentage += Speed / 100.f;
	}
	if (Percentage > MaxScroll)
	{
		Percentage = MaxScroll;
	}
	Window::GetActiveWindow()->UI.RedrawUI();
}

void ScrollObject::ScrollDown()
{
	if (!Active)
	{
		return;
	}
	if (Math::IsPointIn2DBox(Position - Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Percentage -= Speed / 100.f;
	}
	if (Percentage < 0)
		Percentage = 0;
	Window::GetActiveWindow()->UI.RedrawUI();
}
