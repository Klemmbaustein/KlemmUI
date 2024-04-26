#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Window.h>

using namespace KlemmUI;

float UIText::GetRenderedSize() const
{
	float RenderedSize = TextSize;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize / Window::GetActiveWindow()->GetSize().Y * 50 * ParentWindow->GetDPI();
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
		Distance = UIBox::PixelSizeToScreenSize(Vector2f((float)WrapDistance, 0.0), ParentWindow).X;
	}
	return Distance;
}

UIText* UIText::SetFont(Font* NewFont)
{
	if (NewFont != Renderer)
	{
		NewFont = Renderer;
		InvalidateLayout();
	}
	return this;
}

void UIText::Tick()
{
	SetMinSize(GetUsedSize());
}

Vector3f UIText::GetColor() const
{
	return Color;
}

UIText* UIText::SetColor(Vector3f NewColor)
{
	if (Color != NewColor)
	{
		Color = NewColor;
		for (auto& i : RenderedText)
		{
			i.Color = Color;
		}
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
		ParentWindow->UI.RedrawUI();
		this->TextSizeMode = NewMode;
	}
	return this;
}

float UIText::GetTextSize() const
{
	return TextSize / 2;
}

Vector2f UIText::GetTextSizeAtScale(float Scale, SizeMode ScaleType, Font* Renderer)
{
	float RenderedSize = Scale;
	if (ScaleType == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize / Window::GetActiveWindow()->GetSize().Y * 50 * Window::GetActiveWindow()->GetDPI();
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

void UIText::SetText(std::vector<TextSegment> NewText)
{
	if (TextSegment::CombineToString(NewText) != TextSegment::CombineToString(RenderedText))
	{
		RenderedText = NewText;
		if (Wrap)
		{
			if (!Renderer) 
				return;

			Vector2 s = Renderer->GetTextSize(RenderedText, GetRenderedSize(), Wrap, GetWrapDistance());
		}
		InvalidateLayout();
		RedrawElement();
	}
}

size_t UIText::GetNearestLetterAtLocation(Vector2f Location) const
{
	size_t Depth = Renderer->GetCharacterIndexADistance(RenderedText, Location.X - OffsetPosition.X, GetRenderedSize());
	return Depth;
}

std::string UIText::GetText() const
{
	return TextSegment::CombineToString(RenderedText);
}

UIText::UIText(float Scale, Vector3f Color, std::string Text, Font* NewFont) : UIBox(true, Position)
{
	this->TextSize = Scale * 2;
	this->Color = Color;
	this->Renderer = NewFont;
	RenderedText = { TextSegment(Text, Color) };
}

UIText::UIText(float Scale, std::vector<TextSegment> Text, Font* NewFont) : UIBox(true, Position)
{
	this->TextSize = Scale * 2;
	this->Color = Color;
	this->Renderer = NewFont;
	RenderedText = Text;
}

UIText::~UIText()
{
	if (Text) delete Text;
}

Vector2f UIText::GetLetterLocation(size_t Index) const
{
	if (!Renderer) return 0;
	std::string Text = TextSegment::CombineToString(RenderedText);
	return Vector2f(Renderer->GetTextSize({ TextSegment(Text.substr(0, Index), 1) }, GetRenderedSize(), false, 999).X, 0) + OffsetPosition;
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
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - GetRenderedSize() / 40),
			GetRenderedSize(), Color, Opacity, GetWrapDistance());
	}
	else
	{
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - GetRenderedSize() / 40),
			GetRenderedSize(), Color, Opacity, 999);
	}
}

void UIText::OnAttached()
{
}

Vector2f UIText::GetUsedSize()
{
	if (!Renderer)
		return 0;

	Vector2f Size = Renderer->GetTextSize(RenderedText, GetRenderedSize(), Wrap, GetWrapDistance());

	if (TextWidthOverride != 0)
	{
		return Vector2f(TextWidthOverride, Size.Y);
	}
	return Size;
}
