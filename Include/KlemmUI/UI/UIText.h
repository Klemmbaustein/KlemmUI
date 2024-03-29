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
	SizeMode WrapSizeMode = SizeMode::ScreenRelative;
	SizeMode TextSizeMode = SizeMode::AspectRelative;
public:
	UIText* SetTextRenderer(TextRenderer* Font);
	void Tick() override;
	bool Wrap = false;
	float WrapDistance = 0.0f;
	Vector3f32 GetColor() const;
	UIText* SetColor(Vector3f32 NewColor);
	UIText* SetOpacity(float NewOpacity);
	UIText* SetTextSize(float Size);
	UIText* SetTextSizeMode(SizeMode NewMode);
	float GetTextSize() const;
	static Vector2f GetTextSizeAtScale(float Scale, SizeMode ScaleType, TextRenderer* Renderer);
	UIText* SetTextWidthOverride(float NewTextWidthOverride);

	size_t GetNearestLetterAtLocation(Vector2f Location) const;
	Vector2f GetLetterLocation(size_t Index) const;

	UIText* SetWrapEnabled(bool WrapEnabled, float WrapDistance, SizeMode WrapSizeMode);

	void SetText(std::string NewText);
	void SetText(ColoredText NewText);
	std::string GetText() const;
	UIText(float Scale, Vector3f32 Color, std::string Text, TextRenderer* Renderer);
	UIText(float Scale, ColoredText Text, TextRenderer* Renderer);
	virtual ~UIText();
	void Draw() override;
	void Update() override;
	void OnAttached() override;
	Vector2f GetUsedSize() override;
};