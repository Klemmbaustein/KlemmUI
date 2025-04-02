#include <kui/UI/UIDropdown.h>
#include <kui/UI/UIText.h>
#include <kui/App.h>
#include <kui/Window.h>
#include <iostream>
using namespace kui;

UIDropdown::UIDropdown(Vec2f Position,
	UISize Size,
	Vec3f Color,
	Vec3f TextColor,
	std::vector<Option> Options,
	std::function<void(int OptionIndex)> OnClickedFunction,
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
	SetMinSize(SizeVec(Size, 0));

	OptionsBox = new UIBox(false, Position + Vec2(0, -1));
	OptionsBox->SetMinSize(SizeVec(Vec2f(0, 1), SizeMode::AspectRelative));
	OptionsBox->IsVisible = false;
	GenerateOptions();
}

kui::UIDropdown::~UIDropdown()
{
	delete OptionsBox;
}

UIDropdown* UIDropdown::SetTextSize(UISize Size, UISize Padding)
{
	SelectedText->SetTextSize(Size);
	SelectedText->SetPadding(Padding);
	this->TextSize = Size;
	this->TextPadding = Padding;
	return this;
}

UIDropdown* UIDropdown::SetDropdownColor(Vec3f NewColor, Vec3f TextColor)
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
		UIButton* NewButton = new UIButton(true, 0, Vec3f::Lerp(DropdownColor, ButtonColor, (i == SelectedIndex) ? 0.5f : 0), nullptr, (int)i);
		NewButton->SetMinSize(SizeVec(Size, 0));
		NewButton->OnClickedIndex = [this](int Index) {
			OnChildClicked(Index);
			};

		UIText* NewText = new UIText(11_px, DropdownTextColor, Options[i].Name, Renderer);
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
		UIButton::OnButtonClicked();
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
	OptionsBox->SetPosition(OffsetPosition + Vec2(0, -1));
}

void UIDropdown::OnButtonClicked()
{
	OptionsBox->IsVisible = !OptionsBox->IsVisible;
	OptionsBox->MoveToFront();
}

void UIDropdown::OnChildClicked(int Index)
{
	SelectOption((size_t)Index);
	OptionsBox->IsVisible = false;
}