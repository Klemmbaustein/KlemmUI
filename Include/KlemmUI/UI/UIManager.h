#pragma once
#include <set>
#include <vector>

class UIBox;

namespace KlemmUI
{
	class UIManager
	{
	public:
		UIManager();
		~UIManager();

		/**
		* @brief
		* The UI element that is currently hovered.
		*/
		UIBox* HoveredBox = nullptr;
		UIBox* NewHoveredBox = nullptr;

		std::set<UIBox*> ElementsToUpdate;
		std::vector<UIBox*> UIElements;
		bool RequiresRedraw = true;
		unsigned int UIBuffer = 0;
		unsigned int UITexture = 0;

		void ForceUpdateUI();
		void InitUI();
		unsigned int GetUIFramebuffer() const;
		void RedrawUI();
		void ClearUI();
		bool GetShouldRedrawUI();

		bool DrawElements();
	};
}