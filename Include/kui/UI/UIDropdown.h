#pragma once
#include "UIButton.h"
#include "../Font.h"

namespace kui
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
		UISize TextSize, TextPadding;
		std::vector<UIButton*> DropdownButtons;
		std::vector<UIText*> DropdownTexts;
		Vec3f DropdownColor = 1;
		Vec3f DropdownTextColor = 0;
	public:
		SizeMode TextSizeMode = SizeMode::AspectRelative;
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
		UIDropdown(Vec2f Position, float Size, Vec3f Color, Vec3f TextColor, std::vector<Option> Options, std::function<void(int OptionIndex)> OnClickedFunction, Font* TextFont);

		/// Sets the size and padding of the dropdown text.
		UIDropdown* SetTextSize(UISize Size, UISize Padding);
		/// Sets the background and text color of the dropdown.
		UIDropdown* SetDropdownColor(Vec3f NewColor, Vec3f TextColor);

		void Tick() override;
		void OnButtonClicked() override;
		void OnChildClicked(int Index);
	};
}