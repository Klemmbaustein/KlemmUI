#pragma once
#include "UIBox.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "UIStyle.h"

struct VertexBuffer;
class Shader;

class UIButtonStyle : public UIStyle
{
public:
	UIButtonStyle(std::string Name);
	Vector3f32 Color = 1.0f;
	Vector3f32 HoveredColor = 0.75f;
	Vector3f32 PressedColor = 0.5f;

	bool UseTexture = false;
	unsigned int TextureID = 0;

	float Opacity = 1;

protected:
	virtual void ApplyDerived(UIBox* Target) override;
};

class UIButton : public UIBox
{
protected:
	Vector2f Offset;
	VertexBuffer* ButtonVertexBuffer = nullptr;
	bool IsHovered = false;
	bool IsPressed = false;
	Vector3f32 Color = 1.0f;
	Vector3f32 HoveredColor = 0.75f;
	Vector3f32 PressedColor = 0.5f;
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

	void ScrollTick(Shader* UsedShader);
	void MakeGLBuffers();
	void Tick() override;
	virtual void OnClicked();
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
	UIButton* SetHoveredColor(Vector3f32 NewColor);
	UIButton* SetPressedColor(Vector3f32 NewColor);
	Vector3f32 GetColor();

	UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)());
	UIButton(bool Horizontal, Vector2f Position, Vector3f32 Color, void(*PressedFunc)(int), int ButtonIndex);
	UIButton(bool Horizontal, Vector2f Position, UIButtonStyle* Style, void(*PressedFunc)());
	UIButton(bool Horizontal, Vector2f Position, UIButtonStyle* Style, void(*PressedFunc)(int), int ButtonIndex);

	~UIButton();

	void Update() override;
	void Draw() override;
	UIBox* ParentOverride = nullptr;
};