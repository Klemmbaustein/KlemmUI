#pragma once
#include "UIBox.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "UIStyle.h"

class Shader;

class UIBackgroundStyle : public UIStyle
{
public:
	UIBackgroundStyle(std::string Name);
	Vector3f32 Color = 1.0f;

	float Opacity = 1.0f;
	bool UseTexture = false;
	unsigned int TextureID = 0;
	Shader* UsedShadder;
protected:
	virtual void ApplyDerived(UIBox* Target) override;
};


struct VertexBuffer;

class UIBackground : public UIBox
{
	void ScrollTick(Shader* UsedShader);
	void MakeGLBuffers(bool InvertTextureCoordinates = false);
	bool UseTexture = false;
	unsigned int TextureID = 0;
protected:
	Shader* BackgroundShader;
	virtual void DrawBackground();
	Vector3f32 Color;
	VertexBuffer* BoxVertexBuffer = nullptr;
	float Opacity = 1;
	Vector3f32 ColorMultiplier = 1;
public:
	UIBackground* SetOpacity(float NewOpacity);
	float GetOpacity();
	void SetColor(Vector3f32 NewColor);
	Vector3f32 GetColor();
	void SetInvertTextureCoordinates(bool Invert);
	UIBackground* SetUseTexture(bool UseTexture, unsigned int TextureID = 0);
	UIBackground(bool Horizontal, Vector2f Position, Vector3f32 Color, Vector2f MinScale = Vector2f(0), Shader* UsedShader = nullptr);
	virtual ~UIBackground();
	void Draw() override;
	void Update() override;
	void OnAttached() override;
};