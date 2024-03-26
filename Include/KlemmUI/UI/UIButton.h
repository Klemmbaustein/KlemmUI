#pragma once
#include "UIBackground.h"
#include "../Vector2.h"
#include "../Vector3.h"

struct VertexBuffer;
class Shader;

class UIButton : public UIBackground
{
protected:
	Vector2f Offset;
	VertexBuffer* ButtonVertexBuffer = nullptr;
	bool IsHovered = false;
	bool IsPressed = false;
	Vector3f ButtonColor = 1.0f;
	Vector3f HoveredColor = 0.75f;
	Vector3f PressedColor = 0.5f;
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

	Shader* UsedShader = nullptr;
	void Tick() override;
	virtual void OnClicked();
public:
	UIButton* SetOpacity(float NewOpacity);
	float GetOpacity();
	void SetCanBeDragged(bool NewCanBeDragged);
	bool GetIsSelected();
	void SetNeedsToBeSelected(bool NeedsToBeSelected);
	UIButton* SetShader(Shader* NewShader);
	bool GetIsHovered() const;
	bool GetIsPressed() const;
	UIButton* SetUseTexture(bool UseTexture, unsigned int TextureID = 0);
	UIButton* SetColor(Vector3f NewColor);
	UIButton* SetHoveredColor(Vector3f NewColor);
	UIButton* SetPressedColor(Vector3f NewColor);
	Vector3f GetColor();

	UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)());
	UIButton(bool Horizontal, Vector2f Position, Vector3f Color, void(*PressedFunc)(int), int ButtonIndex);

	~UIButton();

	void Update() override;
	void DrawBackground() override;
	UIBox* ParentOverride = nullptr;
};