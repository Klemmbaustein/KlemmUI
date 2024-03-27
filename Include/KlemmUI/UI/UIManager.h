#pragma once
#include <set>
#include <vector>

namespace KlemmUI
{
	class UIBox;
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

		void UpdateEvents();

		struct ButtonEvent
		{
			ButtonEvent(void(*Function)(),
				void(*FunctionIndex)(int),
				UIBox* Btn,
				int Index = 0)
			{
				this->Function = Function;
				this->FunctionIndex = FunctionIndex;
				this->Btn = Btn;
				this->Index = Index;
			}
			void(*Function)() = nullptr;
			void(*FunctionIndex)(int) = nullptr;
			UIBox* Btn = nullptr;
			int Index = 0;
		};
		std::vector<ButtonEvent> ButtonEvents;

	};
}