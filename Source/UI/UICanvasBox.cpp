#include <kui/UI/UICanvasBox.h>

using namespace kui;

kui::UICanvasBox::UICanvasBox(Vec2f Position, UISize Size)
	: UIBox(true, Position)
{
	this->SetMinSize(Size);
	this->SetMaxSize(Size);
}

Vec2f kui::UICanvasBox::UpdateScale()
{
	return GetMinScreenSize();
}

void kui::UICanvasBox::UpdateChildPosition()
{
	for (UIBox* c : Children)
	{
		if (c->IsCollapsed)
		{
			continue;
		}

		Vec2f LocalSpace = ((GetChildPositionValue(c) + 1.0f) / 2.0f * GetMinScreenSize());
		LocalSpace += this->OffsetPosition;

		SetChildPosition(c, LocalSpace);
	}
}
