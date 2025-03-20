#pragma once
#include "UIBox.h"
#include "../Font.h"
#include "../Vec3.h"

namespace kui
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
		Vec3f Color;
		uint32_t MaxLines = UINT32_MAX;
		std::vector<TextSegment> RenderedText;
		UISize TextSize = UISize::Pixels(10);
		DrawableText* Text = nullptr;
		UISize TextWidthOverride = 0;
		float Opacity = 1.0f;
		float GetRenderedSize() const;
		float GetWrapDistance() const;
	public:
		UIText* SetFont(Font* NewFont);
		Font* GetTextFont() const;
		void Tick() override;
		SizeMode WrapSizeMode = SizeMode::ScreenRelative;
		SizeMode TextSizeMode = SizeMode::ScreenRelative;
		UISize WrapDistance = 0.0f;
		bool Wrap = false;
		Vec3f GetColor() const;
		/**
		 * @brief
		 * Sets the color of the text.
		 *
		 * If the text is made of multiple TextSegments, all segments will have the given color assigned.
		 */
		UIText* SetColor(Vec3f NewColor);

		/**
		 * @brief
		 * Sets the opacity of the text to the given value.
		 *
		 * 0 Means completely translucent, 1 means completely visible.
		 */
		UIText* SetOpacity(float NewOpacity);

		/**
		 * @brief
		 * Sets the size of the text.
		 */
		UIText* SetTextSize(UISize Size);

		UISize GetTextSize() const;
		static Vec2f GetTextSizeAtScale(UISize Scale, Font* Renderer);
		UIText* SetTextWidthOverride(UISize NewTextWidthOverride);

		size_t GetNearestLetterAtLocation(Vec2f Location) const;
		Vec2f GetLetterLocation(size_t Index) const;

		UIText* SetMaxWraps(uint32_t NewMaxLines);

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
		 * @return
		 * A pointer to this UIText.
		 */
		UIText* SetWrapEnabled(bool WrapEnabled, UISize WrapDistance);

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
		UIText(UISize Scale, Vec3f Color, std::string Text, Font* Renderer);

		/**
		 * @brief
		 * UIText constructor.
		 *
		 * @param Scale
		 * The size of the text.
		 *
		 * @param Text
		 * The segments of the text.
		 * A text segment contains a color and a string.
		 *
		 * @param Renderer
		 * The font used by the text.
		 */
		UIText(UISize Scale, std::vector<TextSegment> Text, Font* Renderer);
		virtual ~UIText();
		void Draw() override;
		void Update() override;
		void OnAttached() override;
		SizeVec GetUsedSize() override;

	private:
		float PosOffset = 0;
		bool LastWrapEnabled = false;
		bool TextChanged = false;
		float LastRenderSize = 0;
		float LastAspect = 0;
		Vec2f LastSize;
		float LastWrapDistance = 0;
	};
}