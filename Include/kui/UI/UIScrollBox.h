#pragma once
#include "UIBox.h"
#include "../Rendering/ScrollObject.h"

namespace kui
{
	class UIBackground;
	class UIButton;

	/**
	 * @brief
	 * A scroll box.
	 * 
	 * All children of this box can be scrolled through if their combined size exceeds this box's maximum size.
	 */
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

		float OldPercentage = 0;
		float DesiredMaxScroll = 0;
		float GetDesiredChildrenSize();
		void UpdateScrollObjectOfObject(UIBox* o);
	public:
		std::function<void(UIScrollBox* This)> OnScroll;
		
		uint32_t ScrollBarWidth = 10;

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
	};

}