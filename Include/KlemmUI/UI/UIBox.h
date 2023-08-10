#pragma once
#include "../Math/Vector2.h"
#include <set>
#include <vector>

class ScrollObject;
class UIScrollBox;
class UIButton;
class UIBox
{
public:
	bool IsVisible = true;
	enum E_UIAlign
	{
		E_DEFAULT,
		E_CENTERED,
		E_REVERSE
	};
	enum E_BorderType
	{
		E_NONE = 0,
		E_ROUNDED = 1,
		E_DARKENED_EDGE = 2
	};

	enum E_SizeMode
	{
		E_SCREEN_RELATIVE = 0,
		E_PIXEL_RELATIVE = 1
	};

	UIBox* SetSizeMode(E_SizeMode NewMode);

	E_BorderType BorderType;
	float BorderRadius;
	E_UIAlign Align = E_DEFAULT;

	UIBox(bool Horizontal, Vector2f Position);
	virtual ~UIBox();
	virtual void OnAttached();
	void InvalidateLayout();
	UIBox* AddChild(UIBox* NewChild);
	UIBox* GetAbsoluteParent();
	static bool DrawAllUIElements();
	void DrawThisAndChildren();
	void DeleteChildren();

	bool IsVisibleInHierarchy();
	bool IsBeingHovered();

	UIBox* SetMaxSize(Vector2f NewMaxSize);
	Vector2f GetMaxSize();

	UIBox* SetMinSize(Vector2f NewMinSize);
	Vector2f GetMinSize();
	UIBox* SetPosition(Vector2f NewPosition);
	Vector2f GetPosition();
	UIBox* SetPadding(double Up, double Down, double Left, double Right);
	UIBox* SetPadding(double AllDirs);
	UIBox* SetTryFill(bool NewTryFill);
	UIBox* SetHorizontal(bool IsHorizontal);
	bool GetTryFill();
	friend UIScrollBox;
	virtual void OnChildClicked(int Index);
	UIBox* SetBorder(UIBox::E_BorderType Type, double Size);
	static void ForceUpdateUI();
	static void InitUI();
	static unsigned int GetUIFramebuffer();
	static void RedrawUI();
	static void ClearUI();
	static bool GetShouldRedrawUI();
	virtual Vector2f GetUsedSize();
	Vector2f GetScreenPosition();
	void SetCurrentScrollObject(UIScrollBox* s);
	bool IsChildOf(UIBox* Parent);
	bool HasMouseCollision = false;
protected:
	E_SizeMode SizeMode;
	bool ShouldBeTicked = true;
	bool TryFill = false;
	virtual void Update();
	virtual void Draw();
	virtual void Tick();
	virtual void UpdateTickState();
	void UpdateHoveredState();
	bool IsHovered();
	Vector2f Position;
	Vector2f OffsetPosition;
	Vector2f MaxSize = Vector2(999, 999);
	Vector2f MinSize = Vector2(0, 0);

	double UpPadding = 0.01;
	double DownPadding = 0.01;
	double RightPadding = 0.01;
	double LeftPadding = 0.01;
	Vector2f Size;

	std::vector<UIBox*> Children;
	UIBox* Parent = nullptr;
	ScrollObject* CurrentScrollObject = nullptr;
	void UpdateSelfAndChildren();
private:
	bool PrevIsVisible = true;
	void UpdateScale();
	void UpdatePosition();
	bool ChildrenHorizontal;
};

namespace UI
{
	extern UIBox* HoveredBox;
	extern UIBox* NewHoveredBox;

}
