#pragma once
#include "UIBox.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "UIStyle.h"

class UIBackgroundStyle : public UIStyle
{
public:
	UIBackgroundStyle(std::string Name);
	Vector3f32 Color = 1.0f;

	float Opacity = 1.0f;
	bool UseTexture = false;
	unsigned int TextureID = 0;

protected:
	virtual void ApplyDerived(UIBox* Target) override;
};

class Shader;

struct VertexBuffer;

class UIBackground : public UIBox
{
	VertexBuffer* BoxVertexBuffer = nullptr;
	Vector3f32 Color;
	Shader* BackgroundShader;
	void ScrollTick(Shader* UsedShader);
	void MakeGLBuffers(bool InvertTextureCoordinates = false);
	bool UseTexture = false;
	unsigned int TextureID = 0;
	float Opacity = 1;
public:
	UIBackground* SetOpacity(float NewOpacity);
	float GetOpacity();
	void SetColor(Vector3f32 NewColor);
	Vector3f32 GetColor();
	void SetInvertTextureCoordinates(bool Invert);
	UIBackground* SetUseTexture(bool UseTexture, unsigned int TextureID = 0);
	UIBackground(bool Horizontal, Vector2f Position, Vector3f32 Color, Vector2f MinScale = Vector2f(0));
	virtual ~UIBackground();
	void Draw() override;
	void Update() override;
	void OnAttached() override;
};