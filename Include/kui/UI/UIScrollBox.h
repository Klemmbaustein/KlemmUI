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
		float Scrolled = 0;

		float GetOffset() const;

		Vec2f GetPosition() const;
		Vec2f GetScale() const;

		ScrollObject* Parent = nullptr;
		static std::set<ScrollObject*> GetAllScrollObjects();
		ScrollObject(Vec2f Position, Vec2f Scale, float MaxScroll, bool Register = true);
		~ScrollObject();
		void ScrollUp();
		void ScrollDown();
		float Speed = 12;
		bool Active = true;
		float MaxScroll = 10;
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
		static Vec3f BackgroundColor;
		static Vec3f BackgroundBorderColor;
		static Vec3f ScrollBarColor;

		std::function<void(UIScrollBox* This)> OnScroll;

		uint32_t ScrollBarWidth = 10;

		uint32_t ScrollDownPadding = 0;

		bool UseDefaultColors = true;

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

	private:

		ScrollObject ScrollClass = ScrollObject(OffsetPosition, Size, 15);
		UIButton* ScrollBarBackground = nullptr;
		UIBackground* ScrollBar = nullptr;
		float MaxScroll = -1;
		bool IsDragging = false;
		float InitialDragPosition = 0;
		float InitialScrollPosition = 0;
		bool DisplayScrollBar = false;

		float OldPercentage = 0;
		float DesiredMaxScroll = 0;
		float GetDesiredChildrenSize();
		void UpdateScrollObjectOfObject(UIBox* o);

	};

}