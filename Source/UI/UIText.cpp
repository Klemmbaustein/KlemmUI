#include <kui/UI/UIText.h>
#include <kui/App.h>
#include <kui/Window.h>
#include <iostream>
#include <kui/UI/UIScrollBox.h>

using namespace kui;

float UIText::GetRenderedSize() const
{
	return TextSize.GetScreen().Y * 100;
}

float UIText::GetWrapDistance() const
{
	float Distance = WrapDistance.GetScreen().X;
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
	SetMaxSize(GetMinSize());
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
		Update();
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

UIText* UIText::SetTextSize(UISize Size)
{
	if (Size != TextSize)
	{
		TextSize = Size;
		InvalidateLayout();
	}
	return this;
}

Font* UIText::GetTextFont() const
{
	return Renderer;
}

UISize UIText::GetTextSize() const
{
	return TextSize;
}

Vec2f UIText::GetTextSizeAtScale(UISize Scale, Font* Renderer)
{
	return Renderer->GetTextSize({ TextSegment("A", 1) }, Scale.GetScreen().Y * 100, false, 999999, UINT32_MAX);
}

UIText* UIText::SetTextWidthOverride(UISize NewTextWidthOverride)
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
	SetText({ TextSegment(NewText, Color) });
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

std::size_t UIText::GetNearestLetterAtLocation(Vec2f Location) const
{
	if (Renderer == nullptr)
		return 0;

	if (CurrentScrollObject)
	{
		Location.Y -= CurrentScrollObject->GetOffset();
	}

	std::size_t Char = Renderer->GetCharacterAtPosition(RenderedText, Location - OffsetPosition - Vec2f(0, Size.Y),
		GetRenderedSize(), Wrap, GetWrapDistance(), MaxLines);
	return Char;
}

std::string UIText::GetText() const
{
	return TextSegment::CombineToString(RenderedText);
}

UIText::UIText(UISize Scale, Vec3f Color, std::string Text, Font* NewFont) : UIBox(true, 0)
{
	this->TextSize = Scale;
	this->Color = Color;
	this->Renderer = NewFont;
	RenderedText = { TextSegment(Text, Color) };
	TextChanged = true;
}

UIText::UIText(UISize Scale, std::vector<TextSegment> Text, Font* NewFont) : UIBox(true, 0)
{
	this->TextSize = Scale;
	this->Renderer = NewFont;
	RenderedText = Text;
	TextChanged = true;
}

UIText::~UIText()
{
	if (Text)
		delete Text;
}

Vec2f UIText::GetLetterLocation(std::size_t Index) const
{
	if (!Renderer) return 0;
	Vec2f EndLocation;

	Renderer->GetTextSize(RenderedText, GetRenderedSize(), Wrap, GetWrapDistance(), MaxLines, &EndLocation, Index);
	EndLocation.Y = Size.Y - EndLocation.Y;
	return EndLocation + OffsetPosition;
}

UIText* kui::UIText::SetMaxWraps(uint32_t NewMaxLines)
{
	if (NewMaxLines != MaxLines)
	{
		TextChanged = true;
		MaxLines = NewMaxLines;
	}
	return this;
}

UIText* UIText::SetWrapEnabled(bool WrapEnabled, UISize WrapDistance)
{
	this->Wrap = WrapEnabled;
	this->WrapDistance = WrapDistance;
	return this;
}

void UIText::Draw(render::RenderBackend* Backend)
{
	if (!Renderer)
		return;
	if (Text)
	{
		Text->Opacity = Opacity;
		Text->Draw(CurrentScrollObject, OffsetPosition + Vec2f(0, PosOffset));
	}
}

void UIText::Update()
{
	this->Size = GetUsedSize().GetScreen();
}

void UIText::OnAttached()
{
}

SizeVec UIText::GetUsedSize()
{
	if (!Renderer)
		return SizeVec(0, SizeMode::ScreenRelative);

	float RenderSize = GetRenderedSize();
	float WrapDistance = GetWrapDistance();

	Vec2f Size;

	if (RenderSize == LastRenderSize
		&& LastWrapEnabled == Wrap
		&& LastWrapDistance == WrapDistance
		&& ParentWindow->GetAspectRatio() == LastAspect
		&& !TextChanged)
	{
		Size = LastSize;
	}
	else
	{
		Size = Renderer->GetTextSize(RenderedText, RenderSize, Wrap, WrapDistance, MaxLines);
		LastSize = Size;
		LastRenderSize = RenderSize;
		LastWrapEnabled = Wrap;
		LastWrapDistance = WrapDistance;
		LastAspect = ParentWindow->GetAspectRatio();
		TextChanged = false;

		if (Renderer)
		{
			if (Text)
			{
				delete Text;
			}
			PosOffset = Size.Y - GetRenderedSize() / 600 * Renderer->CharacterSize;
			if (Wrap)
			{
				Text = Renderer->MakeText(RenderedText,
					GetRenderedSize(), Color, Opacity, GetWrapDistance(), MaxLines);
			}
			else
			{
				Text = Renderer->MakeText(RenderedText,
					GetRenderedSize(), Color, Opacity, 999, MaxLines);
			}
		}
	}

	if (TextWidthOverride != 0)
	{
		return SizeVec(TextWidthOverride, UISize(Size.Y, SizeMode::ScreenRelative));
	}
	return SizeVec(Size, SizeMode::ScreenRelative);
}
