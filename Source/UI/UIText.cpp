#include <UI/UIText.h>
#include <Application.h>
#include <iostream>


bool UIText::GetRenderHighResMode()
{
	return true;
}

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
	float RenderedSize = TextSize * 2;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
	}
	Vector2 NewMin = Renderer->GetTextSize(RenderedText, RenderedSize, Wrap, WrapDistance);
	if (TextWidthOverride > 0)
	{
		NewMin.X = std::max(MinSize.X, TextWidthOverride);
	}
	SetMinSize(NewMin);
}

Vector3f32 UIText::GetColor() const
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

UIText* UIText::SetTextSizeMode(SizeMode NewMode)
{
	if (this->TextSizeMode != NewMode)
	{
		RedrawUI();
		this->TextSizeMode = NewMode;
	}
	return this;
}

float UIText::GetTextSize()
{
	return TextSize / 2;
}

Vector2f UIText::GetTextSizeAtScale(float Scale, SizeMode ScaleType, TextRenderer* Renderer)
{
	float RenderedSize = Scale * 2;
	if (ScaleType == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
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

void UIText::SetText(ColoredText NewText)
{
	if (TextSegment::CombineToString(NewText) != TextSegment::CombineToString(RenderedText))
	{
		RenderedText = NewText;
		if (Wrap)
		{
			if (!Renderer) return;
			float RenderedSize = TextSize * 2;
			if (TextSizeMode == SizeMode::PixelRelative)
			{
				RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
			}
			float Distance = WrapDistance;
			if (WrapSizeMode == SizeMode::AspectRelative)
			{
				WrapDistance /= Application::AspectRatio;
			}
			if (WrapSizeMode == SizeMode::PixelRelative)
			{
				WrapDistance = UIBox::PixelSizeToScreenSize(Vector2f((double)WrapDistance, 0.0)).X;
			}

			Vector2 s = Renderer->GetTextSize(RenderedText, RenderedSize, Wrap, Distance)
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

size_t UIText::GetNearestLetterAtLocation(Vector2f Location, Vector2f& LetterOutLocation) const
{
	float RenderedSize = TextSize * 2;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
	}
	size_t Depth = Renderer->GetCharacterIndexADistance(RenderedText, Location.X - OffsetPosition.X, RenderedSize, LetterOutLocation);
	LetterOutLocation = LetterOutLocation + OffsetPosition;
	return Depth;
}

std::string UIText::GetText() const
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

Vector2f UIText::GetLetterLocation(size_t Index) const
{
	if (!Renderer) return 0;
	std::string Text = TextSegment::CombineToString(RenderedText);
	float RenderedSize = TextSize * 2;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
	}
	return Vector2f(Renderer->GetTextSize({ TextSegment(Text.substr(0, Index), 1) }, RenderedSize, false, 999).X, 0) + OffsetPosition;
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
		Text->Draw(CurrentScrollObject, GetCurrentUIDepth());
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
	float RenderedSize = TextSize * 2;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
	}
	if (Wrap)
	{
		float Distance = WrapDistance;
		if (WrapSizeMode == SizeMode::AspectRelative)
		{
			WrapDistance /= Application::AspectRatio;
		}
		if (WrapSizeMode == SizeMode::PixelRelative)
		{
			WrapDistance = UIBox::PixelSizeToScreenSize(Vector2f((double)WrapDistance, 0.0)).X;
		}
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - RenderedSize / 40),
			RenderedSize, Color, Opacity, Distance);
	}
	else
	{
		Text = Renderer->MakeText(RenderedText, OffsetPosition + Vector2f(0, Size.Y - RenderedSize / 40),
			RenderedSize, Color, Opacity, 999);
	}
}

void UIText::OnAttached()
{
}

Vector2f UIText::GetUsedSize()
{
	if (!Renderer) return 0;
	float RenderedSize = TextSize * 2;
	if (TextSizeMode == SizeMode::PixelRelative)
	{
		RenderedSize = RenderedSize * 1080 / Application::GetWindowResolution().Y;
	}
	return Renderer->GetTextSize(RenderedText, RenderedSize, Wrap, WrapDistance);
}
