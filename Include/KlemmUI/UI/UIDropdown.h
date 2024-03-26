#pragma once
#include "UIButton.h"

class UIText;
class TextRenderer;

class UIDropdown : public UIButton
{
	TextRenderer* Renderer;
	UIText* SelectedText = nullptr;
	UIBox* OptionsBox = nullptr;
	float Size = 0.0f;
	float TextSize = 0.4f, TextPadding = 0.02f;
	std::vector<UIButton*> DropdownButtons;
	std::vector<UIText*> DropdownTexts;
	Vector3f DropdownColor = 1;
	Vector3f DropdownTextColor = 0;
public:
	size_t SelectedIndex = 0;
	struct Option
	{
		std::string Name;
	};

	Option SelectedOption;

	std::vector<Option> Options;
	void GenerateOptions();

	UIDropdown* SelectOption(size_t Index, bool CallEvent = true);

	UIDropdown(Vector2f Position, float Size, Vector3f Color, Vector3f TextColor, std::vector<Option> Options, void(*PressedFunc)(), TextRenderer* Renderer);
	UIDropdown(Vector2f Position, float Size, Vector3f Color, Vector3f TextColor, std::vector<Option> Options, void(*PressedFunc)(int), int ButtonIndex, TextRenderer* Renderer);
	UIDropdown* SetTextSize(float Size, float Padding);
	UIDropdown* SetDropdownColor(Vector3f NewColor, Vector3f TextColor);
	
	void Tick() override;
	void OnClicked() override;
	void OnChildClicked(int Index) override;
};