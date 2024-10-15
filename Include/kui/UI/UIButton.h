#pragma once
#include "UIBackground.h"
#include "../Vec2.h"
#include "../Vec3.h"
#include <functional>

namespace kui
{

	struct VertexBuffer;
	class Shader;

	/**
	 * @brief
	 * A UI element that functions as a button.
	 */
	class UIButton : public UIBackground
	{
	protected:
		Vec2f Offset;
		VertexBuffer* ButtonVertexBuffer = nullptr;
		bool IsHovered = false;
		bool IsPressed = false;
		bool IsKeyboardFocused = false;
		bool IsSelected = false;
		bool NeedsToBeSelected = false;
		bool CanBeDragged = false;
		Vec3f ButtonColor = 1.0f;
		Vec3f HoveredColor = 0.75f;
		Vec3f PressedColor = 0.5f;
		enum class ButtonState
		{
			Normal,
			Hovered,
			KeyboardHovered,
			Pressed
		};
		ButtonState CurrentButtonState = ButtonState::Normal;
		unsigned int TextureID = 0;
		float Opacity = 1;

		Shader* UsedShader = nullptr;
		void Tick() override;
		virtual void OnButtonClicked();
	public:

		std::function<void()> OnClicked = nullptr;
		std::function<void(int)> OnClickedFunctionIndex = nullptr;
		int ButtonIndex = 0;

		std::function<void(int)> OnDragged = nullptr;

		bool GetIsSelected() const;
		void SetNeedsToBeSelected(bool NeedsToBeSelected);
		/// Returns true of the button is hovered, false if it isn't.
		bool GetIsHovered() const;

		/// Returns true of the button is pressed, false if it isn't.
		bool GetIsPressed() const;

		/**
		 * @brief
		 * Sets the button's normal color, when it isn't hovered or pressed.
		 */
		virtual UIBackground* SetColor(Vec3f NewColor) override;
		/**
		 * @brief
		 * Sets the button's hovered color.
		 */
		UIButton* SetHoveredColor(Vec3f NewColor);
		/**
		 * @brief
		 * Sets the button's pressed color.
		 */
		UIButton* SetPressedColor(Vec3f NewColor);
		virtual Vec3f GetColor() const override;

		/**
		 * @brief
		 * UIButton constructor.
		 * 
		 * @param Horizontal
		 * True if should the children be laid out horizontally, false if vertically.
		 *
		 * @param Position
		 * The position of the UIButton. If it has a parent, this will be ignored.
		 *
		 * @param Color
		 * The color of the UIButton. The hovered and pressed colors will be derived from this.
		 * 
		 * @param OnClicked
		 * This function will be called when the button has been pressed.
		 */
		UIButton(bool Horizontal, Vec2f Position, Vec3f Color, std::function<void()> OnClicked);

		/**
		 * @brief
		 * UIButton constructor.
		 *
		 * @param Horizontal
		 * True if should the children be laid out horizontally, false if vertically.
		 *
		 * @param Position
		 * The position of the UIButton. If it has a parent, this will be ignored.
		 *
		 * @param Color
		 * The color of the UIButton. The hovered and pressed colors will be derived from this.
		 *
		 * @param OnClicked
		 * This function will be called when the button has been pressed.
		 * 
		 * @param ButtonIndex
		 * An index for the button. Will be passed to OnClicked.
		 */
		UIButton(bool Horizontal, Vec2f Position, Vec3f Color, std::function<void(int)> OnClicked, int ButtonIndex);

		~UIButton();

		void Update() override;
		void DrawBackground() override;
		UIBox* ParentOverride = nullptr;
	};
}