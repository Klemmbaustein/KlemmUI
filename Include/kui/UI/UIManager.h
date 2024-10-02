#pragma once
#include <set>
#include <map>
#include <vector>
#include <functional>
#include "../Vec2.h"

namespace kui
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

		Vec2ui ScissorXY, ScissorWH;

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

		/**
		 * @brief
		 * Loads a reference-counted texture.
		 * 
		 * If the same texture file has already been loaded and hasn't been unloaded with UnloadReferenceTexture(),
		 * it will return the ID of that texture.
		 */
		unsigned int LoadReferenceTexture(std::string FilePath);
		/**
		 * @brief
		 * Unloads a texture loaded with LoadReferenceTexture().
		 * 
		 * If the texture has been loaded multiple times
		 */
		void UnloadReferenceTexture(unsigned int TextureID);

		void SetTexturePath(std::string NewPath);

		struct ButtonEvent
		{
			std::function<void()> Function;
			std::function<void(int)> FunctionIndex;
			UIBox* Btn = nullptr;
			int Index = 0;
			ButtonEvent(std::function<void()> Function,
				std::function<void(int)> FunctionIndex,
				UIBox* Btn,
				int Index = 0)
			{
				this->Function = Function;
				this->FunctionIndex = FunctionIndex;
				this->Btn = Btn;
				this->Index = Index;
			}
		};
		std::vector<ButtonEvent> ButtonEvents;

		struct RedrawBox
		{
			Vec2f Min;
			Vec2f Max;

			static bool IsBoxOverlapping(const UIManager::RedrawBox& BoxA, const UIManager::RedrawBox& BoxB);
		};

		std::vector<RedrawBox> RedrawBoxes;
		void RedrawArea(RedrawBox Box);
	};
}