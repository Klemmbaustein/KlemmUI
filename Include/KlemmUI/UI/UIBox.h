#pragma once
#include "../Vector2.h"
#include <set>
#include <vector>

class ScrollObject;
class UIButton;
class UIScrollBox;

class UIBox
{
public:
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
	Align HorizontalBoxAlign = Align::Default;
	Align VerticalBoxAlign = Align::Reverse;

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
	Vector2f GetMaxSize() const;

	UIBox* SetMinSize(Vector2f NewMinSize);
	Vector2f GetMinSize() const;
	UIBox* SetPosition(Vector2f NewPosition);
	Vector2f GetPosition();
	UIBox* SetPadding(float Up, float Down, float Left, float Right);
	UIBox* SetPadding(float AllDirs);
	UIBox* SetPaddingSizeMode(SizeMode NewSizeMode);
	UIBox* SetTryFill(bool NewTryFill);
	UIBox* SetHorizontal(bool IsHorizontal);
	bool GetTryFill() const;
	virtual void OnChildClicked(int Index);
	UIBox* SetBorder(BorderType Type, float Size);
	static void ForceUpdateUI();
	static void InitUI();
	static unsigned int GetUIFramebuffer();
	static void RedrawUI();
	static void ClearUI();
	static bool GetShouldRedrawUI();
	void MoveToFront();
	virtual Vector2f GetUsedSize();
	Vector2f GetScreenPosition() const;
	void SetCurrentScrollObject(UIScrollBox* s);
	void SetCurrentScrollObject(ScrollObject* s);
	bool IsChildOf(UIBox* Parent);
	UIBox* SetHorizontalAlign(Align NewAlign);
	UIBox* SetVerticalAlign(Align NewAlign);
	bool HasMouseCollision = false;
	ScrollObject* CurrentScrollObject = nullptr;

	void GetPadding(Vector2f& UpDown, Vector2f& LeftRight) const;

	const std::vector<UIBox*>& GetChildren();

	void UpdateElement();
	virtual void UpdateTickState();
protected:
	thread_local static std::vector<UIBox*> UIElements;
	
	SizeMode BoxSizeMode = SizeMode::ScreenRelative;
	bool ShouldBeTicked = true;
	bool TryFill = false;
	virtual void Update();
	virtual void Draw();
	virtual void Tick();
	void UpdateHoveredState();
	bool IsHovered();
	Vector2f Position;
	Vector2f OffsetPosition;
	Vector2f MaxSize = Vector2(999, 999);
	Vector2f MinSize = Vector2(0, 0);

	float UpPadding = 0.01f;
	float DownPadding = 0.01f;
	float RightPadding = 0.01f;
	float LeftPadding = 0.01f;
	Vector2f Size;
	SizeMode PaddingSizeMode = SizeMode::ScreenRelative;

	std::vector<UIBox*> Children;
	UIBox* Parent = nullptr;
	void UpdateSelfAndChildren();
	Vector2f GetLeftRightPadding(UIBox* Target);

	static Vector2f PixelSizeToScreenSize(Vector2f PixelSize);

private:
	float GetVerticalOffset();
	float GetHorizontalOffset();
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
