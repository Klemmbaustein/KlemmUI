#pragma once
#include "UIBox.h"
#include "../Rendering/ScrollObject.h"

// TODO: Styles

class UIBackground;
class UIButton;

class UIScrollBox : public UIBox
{
	ScrollObject ScrollClass = ScrollObject(OffsetPosition, Size, 15);
	UIButton* ScrollBarBackground = nullptr;
	UIBackground* ScrollBar = nullptr; 
	float MaxScroll = -1;
	bool IsDragging = false;
	float InitialDragPosition = 0;
	float InitialScrollPosition = 0;
	bool DisplayScrollBar = false;

	float DesiredMaxScroll = 0;
	float GetDesiredChildrenSize();
	void UpdateScrollObjectOfObject(UIBox* o);
public:
	static bool IsDraggingScrollBox;
	ScrollObject* GetScrollObject();
	UIBackground* GetScrollBarSlider();
	UIButton* GetScrollBarBackground();

	UIScrollBox* SetDisplayScrollBar(bool NewDisplay);
	bool GetDiplayScrollBar();
	UIScrollBox* SetScrollSpeed(float NewScrollSpeed);
	float GetScrollSpeed();

	void SetMaxScroll(float NewMaxScroll);
	float GetMaxScroll();
	void Update() override;
	void UpdateTickState() override;
	void Tick() override;
	UIScrollBox(bool Horizontal, Vector2f Position, bool DisplayScrollBar);
	~UIScrollBox();
};