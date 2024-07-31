#pragma once
#include "UIButton.h"
#include "../Rendering/Text/Font.h"

namespace KlemmUI
{
	class UIText;

	/**
	 * @brief
	 * UI dropdown element.
	 */
	class UIDropdown : public UIButton
	{
		Font* Renderer;
		UIText* SelectedText = nullptr;
		UIBox* OptionsBox = nullptr;
		float Size = 0.0f;
		float TextSize = 0.4f, TextPadding = 0.02f;
		std::vector<UIButton*> DropdownButtons;
		std::vector<UIText*> DropdownTexts;
		Vector3f DropdownColor = 1;
		Vector3f DropdownTextColor = 0;
	public:
		UIBox::SizeMode TextSizeMode = UIBox::SizeMode::AspectRelative;
		UIDropdown* SetTextSizeMode(UIBox::SizeMode NewMode);
		size_t SelectedIndex = 0;
		
		/// A dropdown option.
		struct Option
		{
			/// The displayed name of the option.
			std::string Name;
		};

		/// The selected option of the dropdown.
		Option SelectedOption;

		/// All selectable options.
		std::vector<Option> Options;

		/// Updates the selected option and the list of options.
		void GenerateOptions();

		/**
		 * @brief
		 * Selects the given option.
		 * 
		 * @param Index
		 * The index of the option in the Options array.
		 * 
		 * @param CallEvent
		 * Should the OnClicked callback be called.
		 */
		UIDropdown* SelectOption(size_t Index, bool CallEvent = true);

		/**
		 * @brief
		 * UIDropdown constructor.
		 * 
		 * @param Position
		 * The position of the UIDropdown. If it has a parent, this will be ignored.
		 * 
		 * @param Size
		 * The horizontal size of the dropdown.
		 * 
		 * @param Color
		 * The background color of the dropdown.
		 * 
		 * @param TextColor
		 * The text color of the dropdown.
		 * 
		 * @param Options
		 * The dropdown options.
		 * 
		 * @param OnClickedFunction
		 * The function called when an option is selected. The index of the selected option will be given.
		 * 
		 * @param TextFont
		 * The font of the dropdown text.
		 */
		UIDropdown(Vector2f Position, float Size, Vector3f Color, Vector3f TextColor, std::vector<Option> Options, std::function<void(int OptionIndex)> OnClickedFunction, Font* TextFont);

		/// Sets the size and padding of the dropdown text.
		UIDropdown* SetTextSize(float Size, float Padding);
		/// Sets the background and text color of the dropdown.
		UIDropdown* SetDropdownColor(Vector3f NewColor, Vector3f TextColor);

		void Tick() override;
		void OnClicked() override;
		void OnChildClicked(int Index) override;
	};
}