#include <KlemmUI/UI/UIDropdown.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/Window.h>
#include <iostream>
using namespace KlemmUI;

UIDropdown::UIDropdown(Vector2f Position,
	float Size,
	Vector3f Color,
	Vector3f TextColor,
	std::vector<Option> Options,
	void(*OnClickedFunction)(int),
	Font* Renderer)
	:
	UIButton(true,
		Position,
		Color,
		OnClickedFunction, 0)
{
	this->Renderer = Renderer;
	this->Options = Options;
	this->Size = Size;
	if (!this->Options.size())
	{
		this->Options.push_back(Option());
	}
	SelectedOption = Options.at(0);
	SelectedText = new UIText(TextSize, TextColor, this->Options.at(0).Name, Renderer);
	SelectedText->SetPadding(TextPadding);
	AddChild(SelectedText);
	SetMinSize(Vector2f(Size, 0));

	OptionsBox = new UIBox(false, Position + Vector2(0, -1));
	OptionsBox->SetMinSize(Vector2(0, 1));
	OptionsBox->IsVisible = false;
	GenerateOptions();
}

UIDropdown* UIDropdown::SetTextSize(float Size, float Padding)
{
	SelectedText->SetTextSize(Size);
	SelectedText->SetPadding(Padding);
	this->TextSize = Size;
	this->TextPadding = Padding;
	return this;
}

UIDropdown* UIDropdown::SetDropdownColor(Vector3f NewColor, Vector3f TextColor)
{
	if (NewColor != DropdownColor || TextColor != DropdownTextColor)
	{
		for (UIButton* i : DropdownButtons)
		{
			i->SetColor(NewColor);
		}
		for (UIText* i : DropdownTexts)
		{
			i->SetColor(TextColor);
		}
		DropdownColor = NewColor;
		DropdownTextColor = TextColor;
	}
	return this;
}

void UIDropdown::GenerateOptions()
{
	OptionsBox->DeleteChildren();
	for (size_t i = 0; i < Options.size(); i++)
	{
		UIButton* NewButton = new UIButton(true, 0, Vector3f::Lerp(DropdownColor, ButtonColor, (i == SelectedIndex) ? 0.5f : 0), nullptr, (int)i);
		NewButton->SetPadding(0);
		NewButton->SetMinSize(Vector2f(Size, 0));
		NewButton->ParentOverride = this;

		UIText* NewText = new UIText(TextSize, DropdownTextColor, Options[i].Name, Renderer);
		NewText->SetPadding(TextPadding);
		NewButton->AddChild(NewText);

		OptionsBox->AddChild(NewButton);
		DropdownButtons.push_back(NewButton);
	}
}

UIDropdown* UIDropdown::SelectOption(size_t Index, bool CallEvent)
{
	SelectedIndex = Index;
	SelectedOption = Options.at(Index);
	SelectedText->SetText(SelectedOption.Name);
	if (CallEvent)
	{
		ButtonIndex = (int)Index;
		UIButton::OnClicked();
	}
	GenerateOptions();
	return this;
}

void UIDropdown::Tick()
{
	UIButton::Tick();
	if (Window::GetActiveWindow()->Input.IsLMBDown
		&& OptionsBox->IsVisible
		&& (!ParentWindow->UI.HoveredBox || !(ParentWindow->UI.HoveredBox == this || ParentWindow->UI.HoveredBox->IsChildOf(OptionsBox))))
	{
		OptionsBox->IsVisible = false;
	}
	OptionsBox->SetCurrentScrollObject(CurrentScrollObject);
	OptionsBox->SetPosition(OffsetPosition + Vector2(0, -1));
}

void UIDropdown::OnClicked()
{
	OptionsBox->IsVisible = !OptionsBox->IsVisible;
}

void UIDropdown::OnChildClicked(int Index)
{
	SelectOption((size_t)Index);
	OptionsBox->IsVisible = false;
}