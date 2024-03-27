#pragma once
#include "UIBox.h"
#include "../Rendering/Text/Font.h"
#include "../Vector3.h"

namespace KlemmUI
{
	class Shader;
	class DrawableText;

	class UIText : public UIBox
	{
		Font* Renderer = nullptr;
		Vector3f Color;
		ColoredText RenderedText;
		float TextSize = 0.5f;
		DrawableText* Text = nullptr;
		float TextWidthOverride = 0;
		float Opacity = 1.f;
		SizeMode WrapSizeMode = SizeMode::ScreenRelative;
		SizeMode TextSizeMode = SizeMode::AspectRelative;
		float GetRenderedSize() const;
		float GetWrapDistance() const;
	public:
		UIText* SetTextRenderer(Font* Font);
		void Tick() override;
		bool Wrap = false;
		float WrapDistance = 0.0f;
		Vector3f GetColor() const;
		UIText* SetColor(Vector3f NewColor);
		UIText* SetOpacity(float NewOpacity);
		UIText* SetTextSize(float Size);
		UIText* SetTextSizeMode(SizeMode NewMode);
		float GetTextSize() const;
		static Vector2f GetTextSizeAtScale(float Scale, SizeMode ScaleType, Font* Renderer);
		UIText* SetTextWidthOverride(float NewTextWidthOverride);

		size_t GetNearestLetterAtLocation(Vector2f Location) const;
		Vector2f GetLetterLocation(size_t Index) const;

		UIText* SetWrapEnabled(bool WrapEnabled, float WrapDistance, SizeMode WrapSizeMode);

		void SetText(std::string NewText);
		void SetText(ColoredText NewText);
		std::string GetText() const;
		UIText(float Scale, Vector3f Color, std::string Text, Font* Renderer);
		UIText(float Scale, ColoredText Text, Font* Renderer);
		virtual ~UIText();
		void Draw() override;
		void Update() override;
		void OnAttached() override;
		Vector2f GetUsedSize() override;
	};
}