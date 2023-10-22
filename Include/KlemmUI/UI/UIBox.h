#pragma once
#include "../Math/Vector2.h"
#include <set>
#include <vector>

class ScrollObject;
class UIScrollBox;
class UIButton;
class UIStyle;

class UIBox
{
public:
	friend UIScrollBox;
	friend UIStyle;

	bool IsVisible = true;
	enum class Align
	{
		Default,
		Centered,
		Reverse
	};
	enum class BorderType
	{
		None = 0,
		Rounded = 1,
		DarkenedEdge = 2
	};

	enum class SizeMode
	{
		ScreenRelative = 0,
		AspectRelative = 1,
		PixelRelative = 2
	};

	UIBox* SetSizeMode(SizeMode NewMode);

	BorderType BoxBorder = BorderType::None;
	float BorderRadius = 0;
	Align BoxAlign = Align::Default;

	UIBox(bool Horizontal, Vector2f Position);
	UIBox(UIStyle* UsedStyle, bool Horizontal, Vector2f Position);
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
	UIBox* SetPaddingSizeMode(SizeMode NewSizeMode);
	UIBox* SetTryFill(bool NewTryFill);
	UIBox* SetAlign(Align NewAlign);
	UIBox* SetHorizontal(bool IsHorizontal);
	bool GetTryFill();
	virtual void OnChildClicked(int Index);
	UIBox* SetBorder(BorderType Type, double Size);
	static void ForceUpdateUI();
	static void InitUI();
	static unsigned int GetUIFramebuffer();
	static void RedrawUI();
	static void ClearUI();
	static bool GetShouldRedrawUI();
	void MoveToFront();
	virtual Vector2f GetUsedSize();
	Vector2f GetScreenPosition();
	void SetCurrentScrollObject(UIScrollBox* s);
	bool IsChildOf(UIBox* Parent);
	bool HasMouseCollision = false;
protected:
	static std::vector<UIBox*> UIElements;
	UIStyle* UsedStyle = nullptr;
	
	SizeMode BoxSizeMode = SizeMode::ScreenRelative;
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
	SizeMode PaddingSizeMode = SizeMode::ScreenRelative;

	std::vector<UIBox*> Children;
	UIBox* Parent = nullptr;
	ScrollObject* CurrentScrollObject = nullptr;
	void UpdateSelfAndChildren();
	Vector2f GetLeftRightPadding(UIBox* Target);

	static Vector2f PixelSizeToScreenSize(Vector2f PixelSize);

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
