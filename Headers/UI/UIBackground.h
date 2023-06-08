#pragma once
#include "UIBox.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"

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