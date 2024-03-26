#pragma once
#include "UIBackground.h"
#include "../Vector3.h"

class TextRenderer;
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
	UIText* TextObject = nullptr;
	bool IsEdited = false;
	void(*PressedFunc)();
	std::string EnteredText = "";
	float TextSize = 0.5f;
	void Tick() override;
	float TextTimer = 0.0f;
	float ButtonColorMultiplier = 1;
	std::string HintText; // Will be displayed when the text field is empty
	float DoubleClickTimer = 0;
	bool Dragging = false;

	Vector2f TextHighlightPos;
	Vector2f TextHighlightSize;
public:
	UIBox* ParentOverride = nullptr;
	bool GetIsEdited() { return IsEdited; }
	UITextField* SetText(std::string NewText);
	UITextField* SetTextSize(float NewTextSize);
	UITextField* SetTextRenderer(TextRenderer* Font);
	UITextField* SetHintText(std::string NewHintText);
	UITextField* SetColor(Vector3f NewColor);
	Vector3f GetColor();
	UITextField* SetTextColor(Vector3f NewColor);
	Vector3f GetTextColor();
	float GetTextSize();
	std::string GetText();
	bool GetIsHovered();
	bool GetIsPressed();

	UITextField(bool Horizontal, Vector2f Position, Vector3f Color, TextRenderer* Renderer, void(*PressedFunc)());
	~UITextField() override;
	void Update() override;
	void DrawBackground() override;
};
