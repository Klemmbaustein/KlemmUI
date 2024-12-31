#include <kui/UI/UIText.h>
#include <kui/App.h>
#include <kui/Window.h>
#include <kui/Rendering/ScrollObject.h>

using namespace kui;

float UIText::GetRenderedSize() const
{
	float RenderedSize = TextSize;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize / Window::GetActiveWindow()->GetSize().Y * 100 * ParentWindow->GetDPI();
	}
	return RenderedSize;
}

float UIText::GetWrapDistance() const
{
	float Distance = WrapDistance;
	if (WrapSizeMode == SizeMode::AspectRelative)
	{
		Distance /= Window::GetActiveWindow()->GetAspectRatio();
	}
	if (WrapSizeMode == SizeMode::PixelRelative)
	{
		Distance = UIBox::PixelSizeToScreenSize(Vec2f(Distance, 0.0), ParentWindow).X;
	}
	return Distance;
}

UIText* UIText::SetFont(Font* NewFont)
{
	if (NewFont != Renderer)
	{
		Renderer = NewFont;
		InvalidateLayout();
	}
	return this;
}

void UIText::Tick()
{
	SetMinSize(GetUsedSize());
}

Vec3f UIText::GetColor() const
{
	return Color;
}

UIText* UIText::SetColor(Vec3f NewColor)
{
	if (Color != NewColor)
	{
		Color = NewColor;
		for (auto& i : RenderedText)
		{
			i.Color = Color;
		}
		TextChanged = true;
		RedrawElement();
	}
	return this;
}

UIText* UIText::SetOpacity(float NewOpacity)
{
	if (Opacity != NewOpacity)
	{
		Opacity = NewOpacity;
		RedrawElement();
	}
	return this;
}

UIText* UIText::SetTextSize(float Size)
{
	Size *= 2;
	if (Size != TextSize)
	{
		TextSize = Size;
		InvalidateLayout();
	}
	return this;
}

UIText* UIText::SetTextSizeMode(SizeMode NewMode)
{
	if (this->TextSizeMode != NewMode)
	{
		RedrawElement();
		this->TextSizeMode = NewMode;
	}
	return this;
}

Font* UIText::GetTextFont() const
{
	return Renderer;
}

float UIText::GetTextSize() const
{
	return TextSize / 2;
}

Vec2f UIText::GetTextSizeAtScale(float Scale, SizeMode ScaleType, Font* Renderer)
{
	float RenderedSize = Scale * 2;
	if (ScaleType == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize / Window::GetActiveWindow()->GetSize().Y * 100 * Window::GetActiveWindow()->GetDPI();
	}
	return Renderer->GetTextSize({ TextSegment("A", 1) }, RenderedSize, false, 999999);
}

UIText* UIText::SetTextWidthOverride(float NewTextWidthOverride)
{
	if (TextWidthOverride != NewTextWidthOverride)
	{
		TextWidthOverride = NewTextWidthOverride;
		InvalidateLayout();
	}
	return this;
}

void UIText::SetText(std::string NewText)
{
	SetText({TextSegment(NewText, Color)});
}

UIText* UIText::SetText(std::vector<TextSegment> NewText)
{
	if (NewText != RenderedText)
	{
		RenderedText = NewText;
		TextChanged = true;
		InvalidateLayout();
		RedrawElement();
	}
	return this;
}

size_t UIText::GetNearestLetterAtLocation(Vec2f Location) const
{
	if (Renderer == nullptr)
		return 0;
	
	if (CurrentScrollObject)
	{
		Location.Y -= CurrentScrollObject->Percentage;
	}

	size_t Char = Renderer->GetCharacterAtPosition(RenderedText, Location - OffsetPosition - Vec2f(0, Size.Y),
		GetRenderedSize(), Wrap, GetWrapDistance());
	return Char;
}

std::string UIText::GetText() const
{
	return TextSegment::CombineToString(RenderedText);
}

UIText::UIText(float Scale, Vec3f Color, std::string Text, Font* NewFont) : UIBox(true, 0)
{
	this->TextSize = Scale * 2;
	this->Color = Color;
	this->Renderer = NewFont;
	RenderedText = { TextSegment(Text, Color) };
	TextChanged = true;
}

UIText::UIText(float Scale, std::vector<TextSegment> Text, Font* NewFont) : UIBox(true, 0)
{
	this->TextSize = Scale * 2;
	this->Renderer = NewFont;
	RenderedText = Text;
	TextChanged = true;
}

UIText::~UIText()
{
	if (Text) delete Text;
}

Vec2f UIText::GetLetterLocation(size_t Index) const
{
	if (!Renderer) return 0;
	Vec2f EndLocation;

	Renderer->GetTextSize(RenderedText, GetRenderedSize(), Wrap, GetWrapDistance(), &EndLocation, Index);
	EndLocation.Y = Size.Y - EndLocation.Y;
	return EndLocation + OffsetPosition;
}

UIText* UIText::SetWrapEnabled(bool WrapEnabled, float WrapDistance, SizeMode WrapSizeMode)
{
	this->Wrap = WrapEnabled;
	this->WrapDistance = WrapDistance;
	this->WrapSizeMode = WrapSizeMode;
	return this;
}

void UIText::Draw()
{
	if (!Renderer) return;
	if (Text)
	{
		Text->Opacity = Opacity;
		Text->Draw(CurrentScrollObject);
	}
}

void UIText::Update()
{
	if (!Renderer)
	{
		return;
	}
	if (Text)
	{
		delete Text;
	}
	if (Wrap)
	{
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vec2f(0, Size.Y - GetRenderedSize() / 600 * Renderer->CharacterSize),
			GetRenderedSize(), Color, Opacity, GetWrapDistance());
	}
	else
	{
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vec2f(0, Size.Y - GetRenderedSize() / 600 * Renderer->CharacterSize),
			GetRenderedSize(), Color, Opacity, 999);
	}
}

void UIText::OnAttached()
{
}

Vec2f UIText::GetUsedSize()
{
	if (!Renderer)
		return 0;

	float RenderSize = GetRenderedSize();
	float WrapDistance = GetWrapDistance();

	Vec2f Size;

	if (RenderSize == LastRenderSize && LastWrapEnabled == Wrap && LastWrapDistance == WrapDistance && !TextChanged)
	{
		Size = LastSize;
	}
	else
	{
		Size = Renderer->GetTextSize(RenderedText, RenderSize, Wrap, WrapDistance);
		LastSize = Size;
		LastRenderSize = RenderSize;
		LastWrapEnabled = Wrap;
		LastWrapDistance = WrapDistance;
		TextChanged = false;
	}

	if (TextWidthOverride != 0)
	{
		return Vec2f(TextWidthOverride, Size.Y);
	}
	return Size;
}
