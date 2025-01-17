#include <kui/UI/UIScrollBox.h>
#include "../Internal/MathHelpers.h"
#include <kui/UI/UIBox.h>
#include <kui/Window.h>

using namespace kui;

std::set<ScrollObject*> ScrollObject::AllScrollObjects;

std::set<ScrollObject*> ScrollObject::GetAllScrollObjects()
{
	return AllScrollObjects;
}

ScrollObject::ScrollObject(Vec2f Position, Vec2f Scale, float MaxScroll, bool Register)
{
	this->Position = Position;
	this->Scale = Vec2f() - Scale;
	if (Register)
		AllScrollObjects.insert(this);
	this->MaxScroll = MaxScroll;
}

ScrollObject::~ScrollObject()
{
	if (AllScrollObjects.contains(this))
		AllScrollObjects.erase(this);
}

void ScrollObject::ScrollUp()
{
	if (!Active)
	{
		return;
	}
	if (internal::math::IsPointIn2DBox(Position - Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Percentage += Speed / float(Window::GetActiveWindow()->GetSize().Y) * 5.0f;
	}
	if (Percentage > MaxScroll)
	{
		Percentage = MaxScroll;
	}
	Window::GetActiveWindow()->UI.RedrawArea(UIManager::RedrawBox{
		.Min = Position,
		.Max = Position - Scale,
		});
}

void ScrollObject::ScrollDown()
{
	if (!Active)
	{
		return;
	}
	if (internal::math::IsPointIn2DBox(Position - Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Percentage -= Speed / float(Window::GetActiveWindow()->GetSize().Y) * 5.0f;
	}
	if (Percentage < 0)
		Percentage = 0;

	Window::GetActiveWindow()->UI.RedrawArea(UIManager::RedrawBox{
		.Min = Position,
		.Max = Position - Scale,
		});
}
