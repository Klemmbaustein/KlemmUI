#pragma once
#include "UIBox.h"
#include "../Rendering/ScrollObject.h"

class UIScrollBox : public UIBox
{
	ScrollObject ScrollClass = ScrollObject(OffsetPosition, Size, 15);
	float MaxScroll = 15;
	void UpdateScrollObjectOfObject(UIBox* o);
public:
	ScrollObject* GetScrollObject();

	void SetMaxScroll(float NewMaxScroll);
	float GetMaxScroll();
	void Update() override;
	UIScrollBox(bool Horizontal, Vector2f Position, float MaxScroll);
};