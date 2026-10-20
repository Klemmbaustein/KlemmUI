#include <kui/UI/UIStackBox.h>

using namespace kui;

kui::UIStackBox::UIStackBox(Vec2f Position, SizeVec Size)
	: UIBox(false, Position)
{
	SetSize(Size);
}

Vec2f kui::UIStackBox::UpdateScale()
{
	return GetMinScreenSize();
}

void kui::UIStackBox::UpdateChildPosition()
{
	Align PrimaryAlign = ChildrenHorizontal ? HorizontalBoxAlign : VerticalBoxAlign;

	for (UIBox* c : Children)
	{
		if (c->IsCollapsed)
		{
			continue;
		}

		Vec2f UpDown, LeftRight;
		c->GetPaddingScreenSize(UpDown, LeftRight);

		auto ChildSize = c->GetUsedSize().GetScreen();
		float ChildrenSize = ChildrenHorizontal
			? (ChildSize.X + LeftRight.X + LeftRight.Y) : (ChildSize.Y + UpDown.X + UpDown.Y);
		if (PrimaryAlign == Align::Centered)
		{
			if (ChildrenHorizontal)
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(Size.X / 2 - ChildrenSize / 2 + LeftRight.X, c->GetVerticalOffset()));
			}
			else
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(c->GetHorizontalOffset(), Size.Y / 2 - ChildrenSize / 2 + UpDown.Y));
			}
		}
		else if (ChildrenHorizontal)
		{
			if (PrimaryAlign == Align::Reverse)
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(Size.X - ChildSize.X - LeftRight.Y, c->GetVerticalOffset()));
			}
			else
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(LeftRight.X, c->GetVerticalOffset()));
			}
		}
		else
		{
			if (PrimaryAlign == Align::Reverse)
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(c->GetHorizontalOffset(), Size.Y - ChildSize.Y - UpDown.X));
			}
			else
			{
				SetChildPosition(c, OffsetPosition
					+ Vec2f(c->GetHorizontalOffset(), UpDown.Y));
			}
		}
	}
}
