#include <kui/UISize.h>
using namespace kui;

bool kui::UISize::operator==(const UISize& b) const
{
	return Value == b.Value && Mode == b.Mode;
}

UISize kui::UISize::Pixels(float px)
{
	UISize New;
	New.Mode = SizeMode::PixelRelative;
	New.Value = px;
	return New;
}

Vec2f kui::UISize::GetPixels(Window* With) const
{
	switch (Mode)
	{
	case kui::SizeMode::ScreenRelative:
		return Vec2f(Value / 2) * Vec2f(With->GetSize()) / With->GetDPI();
	case kui::SizeMode::AspectRelative:
		return (Vec2f(Value / 2) * Vec2f(With->GetAspectRatio(), 1)) * Vec2f(With->GetSize()) / With->GetDPI();
	case kui::SizeMode::PixelRelative:
		return Value;
	case kui::SizeMode::ParentRelative:
	default:
		break;
	}
	return Value;
}

Vec2f kui::UISize::GetScreen(Window* With) const
{
	switch (Mode)
	{
	case kui::SizeMode::ScreenRelative:
		return Value;
	case kui::SizeMode::AspectRelative:
		return Vec2f(Value) / Vec2f(With->GetAspectRatio(), 1);
	case kui::SizeMode::PixelRelative:
		return Vec2f(Value * 2) / Vec2f(With->GetSize()) * With->GetDPI();
	case kui::SizeMode::ParentRelative:
	default:
		break;
	}
	return Value;
}

kui::SizeVec::SizeVec(UISize X, UISize Y)
{
	this->X = X;
	this->Y = Y;
}

kui::SizeVec::SizeVec(UISize XY)
{
	this->X = XY;
	this->Y = XY;
}

kui::SizeVec::SizeVec(Vec2f XY, SizeMode Mode)
{
	this->X = UISize(XY.X, Mode);
	this->Y = UISize(XY.Y, Mode);
}

kui::SizeVec::SizeVec(Vec2f XY)
	: SizeVec(XY, SizeMode::ScreenRelative)
{
}

kui::SizeVec::SizeVec(float XY)
	: SizeVec(Vec2f(XY))
{
}

SizeVec kui::SizeVec::Pixels(float X, float Y)
{
	return SizeVec(Vec2f(X, Y), SizeMode::PixelRelative);
}

Vec2f kui::SizeVec::GetPixels(Window* With) const
{
	return Vec2f(X.GetPixels().X, Y.GetPixels().Y);
}

Vec2f kui::SizeVec::GetScreen(Window* With) const
{
	return Vec2f(X.GetScreen().X, Y.GetScreen().Y);
}

bool kui::SizeVec::operator==(const SizeVec& other) const
{
	return X == other.X && Y == other.Y;
}

kui::UISize operator""_px(long double i)
{
	return kui::UISize::Pixels(float(i));
}

kui::UISize operator""_px(unsigned long long int i)
{
	return kui::UISize::Pixels(float(i));
}
