#include <kui/UI/UIScrollBox.h>
#include "../Internal/MathHelpers.h"
#include <kui/UI/UIBox.h>
#include <kui/Window.h>

using namespace kui;

thread_local std::set<ScrollObject*> ScrollObject::AllScrollObjects;

Vec2f kui::ScrollObject::GetOffset() const
{
	if (Parent)
		return Scrolled * Vec2f(-1, 1) + Parent->GetOffset();
	return Scrolled * Vec2f(-1, 1);
}

Vec2f kui::ScrollObject::GetPosition() const
{
	if (Parent)
	{
		auto Pos = Position + Parent->GetOffset();
		Pos = Vec2f::Max(Pos, Parent->GetPosition());
		return Pos;
	}
	return Position;
}

Vec2f kui::ScrollObject::GetScale() const
{
	if (Parent)
	{
		Vec2f Pos = GetPosition() + Scale;
		Pos = Vec2f::Min(Pos, Parent->GetScale());
		return Pos;
	}
	return GetPosition() + Scale;
}

std::set<ScrollObject*> ScrollObject::GetAllScrollObjects()
{
	return AllScrollObjects;
}

ScrollObject::ScrollObject(Vec2f Position, Vec2f Scale, Vec2f MaxScroll, bool Register)
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

void ScrollObject::ScrollUp(int Axis)
{
	if (!Active)
	{
		return;
	}
	if (internal::math::IsPointIn2DBox(Position + Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Scrolled[Axis] += Speed / float(Window::GetActiveWindow()->GetSize()[Axis]) * 5.0f;
	}

	Scrolled = Vec2f::Min(Scrolled, MaxScroll);

	Window::GetActiveWindow()->UI.RedrawArea(render::RedrawBox{
		.Min = Position,
		.Max = Position + Scale,
		});
}

void ScrollObject::ScrollDown(int Axis)
{
	if (!Active)
	{
		return;
	}
	if (internal::math::IsPointIn2DBox(Position + Scale, Position, Window::GetActiveWindow()->Input.MousePosition))
	{
		Scrolled[Axis] -= Speed / float(Window::GetActiveWindow()->GetSize()[Axis]) * 5.0f;
	}
	Scrolled = Vec2f::Max(Scrolled, 0.0f);

	Window::GetActiveWindow()->UI.RedrawArea(render::RedrawBox{
		.Min = Position,
		.Max = Position + Scale,
		});
}
