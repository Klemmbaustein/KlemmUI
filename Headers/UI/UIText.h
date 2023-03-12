#pragma once
#include "UIBox.h"
#include "../Rendering/Text/TextRenderer.h"
#include "../Math/Vector3.h"

class Shader;
class DrawableText;
class TextRenderer;

class UIText : public UIBox
{
	TextRenderer* Renderer = nullptr;
	Vector3f32 Color;
	ColoredText RenderedText;
	float TextSize = 0.5f;
	DrawableText* Text = nullptr;
	double TextWidthOverride = 0;
	float Opacity = 1.f;
public:

	bool IsDynamic = false;
	bool Wrap = false;
	float WrapDistance = 0.0f;
	Vector3f32 GetColor();
	UIText* SetColor(Vector3f32 NewColor);
	UIText* SetOpacity(float NewOpacity);
	UIText* SetTextSize(float Size);
	float GetTextSize();
	UIText* SetTextWidthOverride(float NewTextWidthOverride);

	size_t GetNearestLetterAtLocation(Vector2f Location, Vector2f& LetterOutLocation);
	Vector2f GetLetterLocation(size_t Index);

	void SetText(std::string NewText);
	void SetText(ColoredText NewText);
	std::string GetText();
	UIText(float Scale, Vector3f32 Color, std::string Text, TextRenderer* Renderer);
	UIText(float Scale, ColoredText Text, TextRenderer* Renderer);
	virtual ~UIText();
	void Draw() override;
	void Update() override;
	void OnAttached() override;
	Vector2f GetUsedSize() override;
};