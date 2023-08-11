#pragma once
#include "UIBox.h"
#include "../Math/Vector3.h"
#include "UIStyle.h"

class TextRenderer;
class UIText;
struct VertexBuffer;
class Shader;

class UITextFieldStyle : public UIStyle
{
public:
	UITextFieldStyle(std::string Name, TextRenderer* Font);
	Vector3f32 Color = 0.1f;
	Vector3f32 TextColor = 1;
	TextRenderer* Font;
	std::string HintText;
	std::string DefaultText;

	float TextSize = 0.5f;
	bool UseTexture = false;
	unsigned int TextureID = 0;

protected:
	virtual void ApplyDerived(UIBox* Target) override;
};

class UITextField : public UIBox
{
	VertexBuffer* ButtonVertexBuffer = nullptr;
	bool IsHovered = false;
	bool IsPressed = false;
	Vector2f IBeamPosition;
	Vector2f IBeamScale = Vector2(0.001, 0.03);
	bool ShowIBeam = false;
	Vector3f32 Color = Vector3f32(0.5);
	UIText* TextObject = nullptr;
	bool IsEdited = false;
	void(*PressedFunc)();
	std::string EnteredText = "";
	float TextSize = 0.5f;
	void ScrollTick(Shader* UsedShader);
	void MakeGLBuffers();
	void Tick() override;
	float ButtonColorMultiplier = 1;
	std::string HintText; // Will be displayed when the text field is empty
public:
	UIBox* ParentOverride = nullptr;
	bool GetIsEdited() { return IsEdited; }
	UITextField* SetText(std::string NewText);
	UITextField* SetTextSize(float NewTextSize);
	UITextField* SetTextRenderer(TextRenderer* Font);
	UITextField* SetHintText(std::string NewHintText);
	UITextField* SetColor(Vector3f32 NewColor);
	Vector3f32 GetColor();
	UITextField* SetTextColor(Vector3f32 NewColor);
	Vector3f32 GetTextColor();
	float GetTextSize();
	std::string GetText();
	bool GetIsHovered();
	bool GetIsPressed();

	UITextField(bool Horizontal, Vector2f Position, Vector3f32 Color, TextRenderer* Renderer, void(*PressedFunc)());
	UITextField(bool Horizontal, Vector2f Position, UIStyle* Style, void(*PressedFunc)());
	~UITextField() override;
	void Draw() override;
	void Update() override;
};
