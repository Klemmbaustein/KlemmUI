#pragma once
#include <set>
#include <map>
#include <vector>
#include <functional>
#include "../Vec2.h"
#include <cstdint>
#include <kui/Rendering/RedrawArea.h>

namespace kui
{
	class UIBox;
	namespace render
	{
		class RenderBackend;
	}

	class UIManager
	{
		struct ReferenceTexture
		{
			std::string Name;
			std::size_t RefCount = 0;
		};

		std::map<unsigned int, ReferenceTexture> ReferencedTextures;
		std::string TexturePath;

		UIBox* GetNextKeyboardBox(UIBox* From, bool Reverse);
		UIBox* FindKeyboardBox(UIBox* From, bool Reverse);

	public:
		UIManager();
		~UIManager();

		/**
		* @brief
		* The UI element that is currently hovered.
		*/
		UIBox* HoveredBox = nullptr;
		UIBox* NewHoveredBox = nullptr;
		UIBox* KeyboardFocusBox = nullptr;

		std::set<UIBox*> ElementsToUpdate;
		std::vector<UIBox*> UIElements;
		bool RequiresRedraw = true;
		render::RenderBackend* Render = nullptr;

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

		UIBox* GetNextFocusableBox(UIBox* From, bool Direction);

		struct ButtonEvent
		{
			std::function<void()> Function;
			std::function<void(int)> FunctionIndex;
			int Index = 0;
			ButtonEvent(std::function<void()> Function,
				std::function<void(int)> FunctionIndex,
				int Index = 0)
			{
				this->Function = Function;
				this->FunctionIndex = FunctionIndex;
				this->Index = Index;
			}
		};
		std::vector<ButtonEvent> ButtonEvents;

		std::vector<render::RedrawBox> RedrawBoxes;
		void RedrawArea(render::RedrawBox Box);
	};
}