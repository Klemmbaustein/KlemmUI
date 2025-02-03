#include <kui/UI/UIScrollBox.h>
#include "../Internal/MathHelpers.h"
#include <kui/UI/UIBox.h>
#include <kui/Window.h>

using namespace kui;

std::set<ScrollObject*> ScrollObject::AllScrollObjects;

float kui::ScrollObject::GetOffset() const
{
	if (Parent)
		return Scrolled + Parent->GetOffset();
	return Scrolled;
}

Vec2f kui::ScrollObject::GetPosition() const
{
	if (Parent)
	{
		auto Pos = Position + Vec2f(0, Parent->GetOffset());
		Pos.Y = std::max(Pos.Y, Parent->GetPosition().Y);
		return Pos;
	}
	return Position;
}

Vec2f kui::ScrollObject::GetScale() const
{
	if (Parent)
	{
		float Pos = GetPosition().Y + Scale.Y;
		Pos = std::min(Pos, Parent->GetScale().Y);
		return Vec2f(Scale.X, Pos);
	}
	return Vec2f(0, GetPosition().Y) + Scale;
}

std::set<ScrollObject*> ScrollObject::GetAllScrollObjects()
{
	return AllScrollObjects;
}

ScrollObject::ScrollObject(Vec2f Position, Vec2f Scale, float MaxScroll, bool Register)
{
	this->Position = Position;
	this->Scale = Scale;
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
	if (internal::math::IsPointIn2DBox(Position + Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Scrolled += Speed / float(Window::GetActiveWindow()->GetSize().Y) * 5.0f;
	}

	Scrolled = std::min(Scrolled, MaxScroll);

	Window::GetActiveWindow()->UI.RedrawArea(UIManager::RedrawBox{
		.Min = Position,
		.Max = Position + Scale,
		});
}

void ScrollObject::ScrollDown()
{
	if (!Active)
	{
		return;
	}
	if (internal::math::IsPointIn2DBox(Position + Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Scrolled -= Speed / float(Window::GetActiveWindow()->GetSize().Y) * 5.0f;
	}
	Scrolled = std::max(Scrolled, 0.0f);

	Window::GetActiveWindow()->UI.RedrawArea(UIManager::RedrawBox{
		.Min = Position,
		.Max = Position + Scale,
		});
}
