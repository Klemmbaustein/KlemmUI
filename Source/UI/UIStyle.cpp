#include <KlemmUI/UI/UIStyle.h>

UIStyle::UIStyle(std::string Name)
{
	this->Name = Name;
}


UIStyle::~UIStyle()
{
	for (UIBox* i : UIBox::UIElements)
	{
		if (i->UsedStyle == this)
		{
			i->UsedStyle = nullptr;
		}
	}
}

UIStyle* UIStyle::SetPadding(double Up, double Down, double Left, double Right)
{
	UpPadding = Up;
	DownPadding = Down;
	LeftPadding = Left;
	RightPadding = Right;
	return this;
}

UIStyle* UIStyle::SetPadding(double AllDirs)
{
	UpPadding = AllDirs;
	DownPadding = AllDirs;
	RightPadding = AllDirs;
	LeftPadding = AllDirs;
	return this;
}

void UIStyle::ApplyTo(UIBox* Target)
{
	Target->SetPadding(UpPadding, DownPadding, LeftPadding, RightPadding);
	Target->SetBorder(Border, BorderSize);
	Target->SetMinSize(MinSize);
	Target->SetMaxSize(MaxSize);
	ApplyDerived(Target);
}

void UIStyle::UpdateStyle()
{
	for (UIBox* i : UIBox::UIElements)
	{
		if (i->UsedStyle == this)
		{
			ApplyTo(i);
		}
	}
}

void UIStyle::ApplyDerived(UIBox* Target)
{
}
