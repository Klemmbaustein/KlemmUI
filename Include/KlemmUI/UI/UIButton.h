#pragma once
#include "UIBox.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

struct VertexBuffer;
class Shader;

class UIButton : public UIBox
{
	float ButtonColorMultiplier = 1.f;
	Vector2f Offset;
	VertexBuffer* ButtonVertexBuffer = nullptr;
	bool IsHovered = false;
	bool IsPressed = false;
	Vector3f32 Color = Vector3(0.5);
	void(*PressedFunc)() = nullptr;
	void(*PressedFuncIndex)(int) = nullptr;
	bool UseTexture = false;
	unsigned int TextureID = 0;
	bool IsSelected = false;
	bool NeedsToBeSelected = false;
	bool CanBeDragged = false;
	float Opacity = 1;
	int ButtonIndex = 0;

	void ScrollTick(Shader* UsedShader);
	void MakeGLBuffers();
	void Tick() override;
public:
	UIButton* SetOpacity(float NewOpacity);
	float GetOpacity();
	void SetCanBeDragged(bool NewCanBeDragged);
	bool GetIsSelected();
	void SetNeedsToBeSelected(bool NeedsToBeSelected);
	bool GetIsHovered();
	bool GetIsPressed();
	UIButton* SetUseTexture(bool UseTexture, unsigned int TextureID = 0);
	UIButton* SetColor(Vector3f32 NewColor);
	Vector3f32 GetColor();

	UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)());
	UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)(int), int ButtonIndex);
	~UIButton();

	void Update() override;
	void Draw() override;
	UIBox* ParentOverride = nullptr;
};