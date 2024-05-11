#pragma once
#include "UIBox.h"
#include "../Rendering/Text/Font.h"
#include "../Vector3.h"

namespace KlemmUI
{
	class Shader;
	class DrawableText;

	/**
	 * @brief
	 * An UI element containing a text string.
	 */
	class UIText : public UIBox
	{
		Font* Renderer = nullptr;
		Vector3f Color;
		std::vector<TextSegment> RenderedText;
		float TextSize = 0.5f;
		DrawableText* Text = nullptr;
		float TextWidthOverride = 0;
		float Opacity = 1.f;
		SizeMode TextSizeMode = SizeMode::AspectRelative;
		float GetRenderedSize() const;
		float GetWrapDistance() const;
	public:
		UIText* SetFont(Font* NewFont);
		void Tick() override;
		SizeMode WrapSizeMode = SizeMode::ScreenRelative;
		bool Wrap = false;
		float WrapDistance = 0.0f;
		Vector3f GetColor() const;
		/**
		 * @brief
		 * Sets the color of the text.
		 * 
		 * If the text is made of multiple TextSegments, all segments will have the given color assigned.
		 */
		UIText* SetColor(Vector3f NewColor);
		UIText* SetOpacity(float NewOpacity);

		/**
		 * @brief
		 * Sets the size of the text.
		 */
		UIText* SetTextSize(float Size);

		/**
		 * @brief
		 * Sets the size mode of the text.
		 * 
		 * > Note: This has to be used instead of UIBox::SetSizeMode()
		 */
		UIText* SetTextSizeMode(SizeMode NewMode);
		float GetTextSize() const;
		static Vector2f GetTextSizeAtScale(float Scale, SizeMode ScaleType, Font* Renderer);
		UIText* SetTextWidthOverride(float NewTextWidthOverride);

		size_t GetNearestLetterAtLocation(Vector2f Location) const;
		Vector2f GetLetterLocation(size_t Index) const;

		/**
		 * @brief
		 * Enables or disables text wrapping with the given distance and size mode.
		 * 
		 * @param WrapEnabled
		 * If this is true, text wrapping will be enabled.
		 * 
		 * @param WrapDistance
		 * The distance at which the text should be wrapped.
		 * 
		 * @param WrapSizeMode
		 * The size mode of the wrap distance. See size mode for more info.
		 * 
		 * @return
		 * A pointer to this UIText.
		 */
		UIText* SetWrapEnabled(bool WrapEnabled, float WrapDistance, SizeMode WrapSizeMode);

		/**
		 * @brief
		 * Sets the text string of this UIText.
		 */
		void SetText(std::string NewText);

		/**
		 * @brief
		 * Sets the text of this UIText from a list of TextSegments.
		 */
		UIText* SetText(std::vector<TextSegment> NewText);
		std::string GetText() const;

		/**
		 * @brief
		 * UIText constructor.
		 * 
		 * @param Scale
		 * The size of the text.
		 * 
		 * @param Color
		 * The color of the text.
		 * 
		 * @param Text
		 * The displayed color of the text.
		 * 
		 * @param Renderer
		 * The font used by the text.
		 */
		UIText(float Scale, Vector3f Color, std::string Text, Font* Renderer);

		/**
		 * @brief
		 * UIText constructor.
		 * 
		 * @param Scale
		 * The size of the text.
		 * 
		 * @param Color
		 * The segments of the text.
		 * 
		 * A text segment contains a color and a string.
		 * 
		 * @param Renderer
		 * The font used by the text.
		 */
		UIText(float Scale, std::vector<TextSegment> Text, Font* Renderer);
		virtual ~UIText();
		void Draw() override;
		void Update() override;
		void OnAttached() override;
		Vector2f GetUsedSize() override;
	};
}