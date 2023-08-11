#include <UI/UIText.h>
#include <Application.h>


UIText* UIText::SetTextRenderer(TextRenderer* Font)
{
	if (Font != Renderer)
	{
		Font = Renderer;
		InvalidateLayout();
	}
	return this;
}

void UIText::Tick()
{
	if (!Renderer) return;
	Vector2 NewMin = Renderer->GetTextSize(RenderedText, TextSize * 2, Wrap, WrapDistance);
	if (TextWidthOverride > 0)
	{
		NewMin.X = std::max(MinSize.X, TextWidthOverride);
	}
	SetMinSize(NewMin);
}

Vector3f32 UIText::GetColor()
{
	return Color;
}

UIText* UIText::SetColor(Vector3f32 NewColor)
{
	if (Color != NewColor)
	{
		Color = NewColor;
		for (auto& i : RenderedText)
		{
			i.Color = Color;
		}
		RedrawUI();
	}
	return this;
}

UIText* UIText::SetOpacity(float NewOpacity)
{
	if (Opacity != NewOpacity)
	{
		Opacity = NewOpacity;
		RedrawUI();
	}
	return this;
}

UIText* UIText::SetTextSize(float Size)
{
	if (Size * 2 != TextSize)
	{
		TextSize = Size * 2;
		InvalidateLayout();
	}
	return this;
}

float UIText::GetTextSize()
{
	return TextSize / 2;
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
	if (NewText != TextSegment::CombineToString(RenderedText))
	{
		RenderedText = { TextSegment(NewText, Color) };
		if (Wrap)
		{
			if (!Renderer) return;
			Vector2 s = Renderer->GetTextSize(RenderedText, TextSize * 2, Wrap, WrapDistance)
				/ ((30 + Renderer->CharacterSizeInPixels / 2) * 60.f);
			if (s.X < WrapDistance)
			{
				Update();
				RedrawUI();
				return;
			}
		}
		InvalidateLayout();
	}
}

void UIText::SetText(ColoredText NewText)
{
	if (NewText != RenderedText)
	{
		RenderedText = NewText;
		if (Wrap)
		{
			if (!Renderer) return;
			Vector2 s = Renderer->GetTextSize(RenderedText, TextSize * 2, Wrap, WrapDistance)
				/ ((30 + Renderer->CharacterSizeInPixels / 2) * 60.f);
			if (s.X < WrapDistance)
			{
				Update();
				RedrawUI();
				return;
			}
		}
		InvalidateLayout();
	}
}

size_t UIText::GetNearestLetterAtLocation(Vector2f Location, Vector2f& LetterOutLocation)
{
	size_t Depth = Renderer->GetCharacterIndexADistance(RenderedText, Location.X - OffsetPosition.X, TextSize * 2, LetterOutLocation);
	LetterOutLocation = LetterOutLocation + OffsetPosition;
	return Depth;
}

std::string UIText::GetText()
{
	return TextSegment::CombineToString(RenderedText);
}

UIText::UIText(float Scale, Vector3f32 Color, std::string Text, TextRenderer* Renderer) : UIBox(true, Position)
{
	this->TextSize = Scale * 2;
	this->Color = Color;
	this->Renderer = Renderer;
	RenderedText = { TextSegment(Text, Color) };
}

UIText::UIText(float Scale, ColoredText Text, TextRenderer* Renderer) : UIBox(true, Position)
{
	this->TextSize = Scale * 2;
	this->Color = Color;
	this->Renderer = Renderer;
	RenderedText = Text;
}

UIText::~UIText()
{
	if (Text) delete Text;
}

Vector2f UIText::GetLetterLocation(size_t Index)
{
	if (!Renderer) return 0;
	std::string Text = TextSegment::CombineToString(RenderedText);
	return Vector2f(Renderer->GetTextSize({ TextSegment(Text.substr(0, Index), 1) }, TextSize * 2, false, 999).X, 0) + OffsetPosition;
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
	if (!Renderer) return;
	if (Text) delete Text;
	if (Wrap)
	{
		float Distance = WrapDistance;
		if (WrapSizeMode == SizeMode::PixelRelative)
		{
			WrapDistance /= Application::AspectRatio;
		}
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - TextSize / 20),
			TextSize * 2, Color, Opacity, Distance);
	}
	else
	{
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - TextSize / 20),
			TextSize * 2, Color, Opacity, 999);
	}
}

void UIText::OnAttached()
{
}

Vector2f UIText::GetUsedSize()
{
	if (!Renderer) return 0;
	return Renderer->GetTextSize(RenderedText, TextSize * 2, Wrap, WrapDistance);
}
