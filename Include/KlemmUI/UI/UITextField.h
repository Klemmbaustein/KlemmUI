#pragma once
#include "UIBackground.h"
#include "../Vector3.h"

namespace KlemmUI
{

	class Font;
	class UIText;
	struct VertexBuffer;
	class Shader;

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
		void(*PressedFunc)() = nullptr;
		std::string EnteredText = "";
		float TextSize = 0.5f;
		void Tick() override;
		float TextTimer = 0.0f;
		std::string HintText; // Will be displayed when the text field is empty
		bool Dragging = false;

		Vector2f TextHighlightPos;
		Vector2f TextHighlightSize;
	public:
		UIBox* ParentOverride = nullptr;
		bool GetIsEdited() const { return IsEdited; }
		UITextField* SetText(std::string NewText);
		UITextField* SetTextSize(float NewTextSize);
		UITextField* SetTextRenderer(Font* Font);
		UITextField* SetHintText(std::string NewHintText);
		UITextField* SetColor(Vector3f NewColor);
		Vector3f GetColor() const;
		UITextField* SetTextColor(Vector3f NewColor);
		Vector3f GetTextColor();
		float GetTextSize() const;
		std::string GetText();
		bool GetIsHovered() const;
		bool GetIsPressed() const;

		UITextField(Vector2f Position, Vector3f Color, Font* Renderer, void(*PressedFunc)());
		~UITextField() override;
		void Update() override;
		void DrawBackground() override;
	};
}