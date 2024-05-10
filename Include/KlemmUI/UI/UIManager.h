#pragma once
#include <set>
#include <map>
#include <vector>
#include "../Vector2.h"

namespace KlemmUI
{
	class UIBox;
	class UIManager
	{
		struct ReferenceTexture
		{
			std::string Name;
			size_t RefCount = 0;
		};

		std::map<unsigned int, ReferenceTexture> ReferencedTextures;
		std::string TexturePath;

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
		bool GetShouldRedrawUI() const;

		bool DrawElements();
		void TickElements();

		void UpdateEvents();

		unsigned int LoadReferenceTexture(std::string FilePath);
		void UnloadReferenceTexture(unsigned int TextureID);

		void SetTexturePath(std::string NewPath);

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

		struct RedrawBox
		{
			Vector2f Min;
			Vector2f Max;

			static bool IsBoxOverlapping(const UIManager::RedrawBox& BoxA, const UIManager::RedrawBox& BoxB);
		};

		std::vector<RedrawBox> RedrawBoxes;
		void RedrawArea(RedrawBox Box);
	};
}