#pragma once
#include "UIBackground.h"
#include "../Vector2.h"
#include "../Vector3.h"

namespace KlemmUI
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
		Vector2f Offset;
		VertexBuffer* ButtonVertexBuffer = nullptr;
		bool IsHovered = false;
		bool IsPressed = false;
		Vector3f ButtonColor = 1.0f;
		Vector3f HoveredColor = 0.75f;
		Vector3f PressedColor = 0.5f;
		enum class ButtonState
		{
			Normal,
			Hovered,
			Pressed
		};
		ButtonState CurrentButtonState = ButtonState::Normal;

		void(*PressedFunc)() = nullptr;
		void(*PressedFuncIndex)(int) = nullptr;
		bool UseTexture = false;
		unsigned int TextureID = 0;
		bool IsSelected = false;
		bool NeedsToBeSelected = false;
		bool CanBeDragged = false;
		float Opacity = 1;
		int ButtonIndex = 0;

		Shader* UsedShader = nullptr;
		void Tick() override;
		virtual void OnClicked();
	public:
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
		virtual UIBackground* SetColor(Vector3f NewColor) override;
		/**
		 * @brief
		 * Sets the button's hovered color.
		 */
		UIButton* SetHoveredColor(Vector3f NewColor);
		/**
		 * @brief
		 * Sets the button's pressed color.
		 */
		UIButton* SetPressedColor(Vector3f NewColor);
		virtual Vector3f GetColor() override;

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
		 * @param PressedFunc
		 * This function will be called when the button has been pressed.
		 */
		UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)());

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
		 * @param PressedFunc
		 * This function will be called when the button has been pressed.
		 * 
		 * @param ButtonIndex
		 * An index for the button. Will be passed to PressedFunc.
		 */
		UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)(int), int ButtonIndex);

		~UIButton();

		void Update() override;
		void DrawBackground() override;
		UIBox* ParentOverride = nullptr;
	};
}