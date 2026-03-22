#pragma once
#include "UIBox.h"
#include <kui/Vec3.h>

namespace kui
{
	class UIBackground;
	class UIButton;

	class ScrollObject
	{
		thread_local static std::set<ScrollObject*> AllScrollObjects;
	public:
		Vec2f Position;
		Vec2f Scale;
		Vec2f Scrolled = 0;

		Vec2f GetOffset() const;

		Vec2f GetPosition() const;
		Vec2f GetScale() const;

		ScrollObject* Parent = nullptr;
		static std::set<ScrollObject*> GetAllScrollObjects();
		ScrollObject(Vec2f Position, Vec2f Scale, Vec2f MaxScroll, bool Register = true);
		~ScrollObject();
		void ScrollUp(int Axis);
		void ScrollDown(int Axis);
		float Speed = 12;
		bool Active = true;
		Vec2f MaxScroll = 10;
	};

	/**
	 * @brief
	 * A scroll box.
	 *
	 * All children of this box can be scrolled through if their combined size exceeds this box's maximum size.
	 */
	class UIScrollBox : public UIBox
	{
	public:

		std::function<void(UIScrollBox* This)> OnScroll;

		uint32_t ScrollBarWidth = 10;

		uint32_t ScrollDownPadding = 0;

		bool UseDefaultColors = true;
		bool IncludeScrollBarInScroll = false;

		static bool IsDraggingScrollBox;
		ScrollObject* GetScrollObject();
		UIBackground* GetScrollBarSlider();
		UIButton* GetScrollBarBackground();

		UIScrollBox* SetDisplayScrollBar(bool NewDisplay);
		bool GetDisplayScrollBar() const;
		UIScrollBox* SetScrollSpeed(float NewScrollSpeed);
		float GetScrollSpeed() const;

		void SetMaxScroll(float NewMaxScroll);
		float GetMaxScroll() const;
		void Update() override;
		void UpdateTickState() override;
		void Tick() override;
		UIScrollBox(bool Horizontal, Vec2f Position, bool DisplayScrollBar);
		~UIScrollBox();

		void UpdateColors();

	private:

		ScrollObject ScrollClass = ScrollObject(OffsetPosition, Size, 15);
		UIButton* ScrollBarBackground = nullptr;
		UIBackground* ScrollBar = nullptr;
		Vec2f MaxScroll = -1;
		bool IsDragging = false;
		float InitialDragPosition = 0;
		float InitialScrollPosition = 0;
		bool DisplayScrollBar = false;

		Vec2f OldPercentage = 0;
		Vec2f DesiredMaxScroll = 0;
		Vec2f GetDesiredChildrenSize();
		void UpdateScrollObjectOfObject(UIBox* o);

	};

}