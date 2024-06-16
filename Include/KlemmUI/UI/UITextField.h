#pragma once
#include "UIBackground.h"
#include "../Vector3.h"

namespace KlemmUI
{

	class Font;
	class UIText;
	struct VertexBuffer;
	class Shader;
	/**
	 * @brief
	 * Functions like a UIButton, but you can input text into it when clicking it.
	 *
	 * The Text field can have a color and opacity.
	 */
	class UITextField : public UIBackground
	{
		bool IsHovered = false;
		bool IsPressed = false;
		Vector2f IBeamPosition;
		Vector2f IBeamScale = Vector2(0.001, 0.03);
		bool ShowIBeam = false;
		Vector3f TextColor = Vector3f(1);
		Vector3f TextFieldColor = Vector3f(1);
		UIText* TextObject = nullptr;
		bool IsEdited = false;
		std::string EnteredText = "";
		float TextSize = 0.5f;
		void Tick() override;
		float TextTimer = 0.0f;
		std::string HintText; // Will be displayed when the text field is empty
		bool Dragging = false;

		Vector2f TextHighlightPos;
		Vector2f TextHighlightSize;
	public:
		std::function<void()> OnClickedFunction = nullptr;
		UIBox* ParentOverride = nullptr;
		bool GetIsEdited() const { return IsEdited; }
		UITextField* SetText(std::string NewText);
		UITextField* SetTextSize(float NewTextSize);

		/**
		 * @brief
		 * Sets the font used by the text field to the given font.
		 */
		UITextField* SetFont(Font* NewFont);
		UITextField* SetHintText(std::string NewHintText);
		UITextField* SetColor(Vector3f NewColor);
		Vector3f GetColor() const;
		UITextField* SetTextColor(Vector3f NewColor);
		Vector3f GetTextColor();
		float GetTextSize() const;
		std::string GetText();
		bool GetIsHovered() const;
		bool GetIsPressed() const;

		KlemmUI::UITextField* SetTextSizeMode(UIBox::SizeMode Mode);

		/**
		 * @brief
		 * UITextField constructor.
		 * 
		 * @param Position
		 * The position of the text field.
		 * 
		 * @param Color
		 * The background color of the text field.
		 * 
		 * @param Renderer
		 * The 
		 */
		UITextField(Vector2f Position, Vector3f Color, Font* Renderer, std::function<void()> OnClickedFunction);
		~UITextField() override;
		void Update() override;
		void DrawBackground() override;
	};
}