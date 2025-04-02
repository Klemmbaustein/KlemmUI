#include "Values.h"
#include <functional>
#include <kui/DynamicMarkup.h>
#include <kui/Window.h>
#include <typeinfo>
#include <iostream>
#include <kui/UI/UIBackground.h>
#include <kui/UI/UIButton.h>
#include <kui/UI/UIBlurBackground.h>
#include <kui/UI/UITextField.h>
#include <kui/UI/UIText.h>
#include <kui/UI/UISpinner.h>
#include <kui/UI/UIScrollBox.h>
#include <typeinfo>
using namespace kui;

struct DynamicProperty
{
	std::string Name;
	std::function<void(UIBox* Target, kui::AnyContainer Value)> SetValue;
	std::function<bool(UIBox* Target)> IsTarget;
};

UIBox::Align ConvertAlign(std::string Name)
{
	if (Name == "default")
		return UIBox::Align::Default;
	if (Name == "centered")
		return UIBox::Align::Centered;
	if (Name == "reverse")
		return UIBox::Align::Reverse;
	return UIBox::Align::Default;
}

static std::string EscapeString(std::string Value)
{
	if (Value.size() < 2)
		return "";

	if (Value[0] != '"' || Value[Value.size() - 1] != '"')
		return "";

	Value = Value.substr(1, Value.size() - 2);

	std::string OutValue;

	char Last = 0;
	for (char c : Value)
	{
		if (c == '\\')
		{
			if (Last == '\\')
				OutValue.push_back(c);
		}
		else if (Last == '\\')
		{
			if (c == 'n')
			{
				OutValue.push_back('\n');
			}
		}
		else
		{
			OutValue.push_back(c);
		}
		Last = c;
	}

	return OutValue;
}

#include "MarkupProperties.h"

UIBox* kui::markup::CreateNew(std::string Name, DynamicMarkupContext* From)
{
	if (Name == "UIBox")
		return new UIBox(true, 0);
	if (Name == "UIBackground")
		return new UIBackground(true, 0, 1);
	if (Name == "UIScrollBox")
		return new UIScrollBox(true, 0, true);
	if (Name == "UIButton")
		return new UIButton(true, 0, 1, nullptr);
	if (Name == "UIText")
		return new UIText(0.1f, 1, "", Window::GetActiveWindow()->Markup.GetFont(""));
	if (Name == "UITextField")
		return new UITextField(0, 1, Window::GetActiveWindow()->Markup.GetFont(""), nullptr);

	return new UIDynMarkupBox(From, Name);
}

void kui::markup::ApplyElementValues(UIBox* Box, MarkupElement& Target, DynamicMarkupContext* From, UIDynMarkupBox* Root)
{
	ApplyElementValues(Box, Target, Target.Root, From, Root);
}

void kui::markup::ApplyElementValues(UIBox* Box, MarkupElement& TargetElement, UIElement Target, DynamicMarkupContext* From, UIDynMarkupBox* Root)
{
	for (auto& i : Target.ElementProperties)
	{
		SetUIBoxValue(Box, i.Name, i.Value, From, Root, TargetElement);
	}

	for (auto& Child : Target.Children)
	{
		UIBox* New = CreateNew(Child.TypeName.Text, From);
		if (!Child.ElementName.Empty())
		{
			Root->NamedChildren[Child.ElementName.Text] = New;
		}
		ApplyElementValues(New, TargetElement, Child, From, Root);
		Box->AddChild(New);
	}
}

static SizeMode ConvertSizeMode(std::string Mode)
{
	if (Mode == "pr" || Mode == "px")
	{
		return SizeMode::PixelRelative;
	}
	if (Mode == "ar")
	{
		return SizeMode::AspectRelative;
	}
	if (Mode == "pn" || Mode == "%")
	{
		return SizeMode::ParentRelative;
	}
	return SizeMode::ScreenRelative;
}

static Vec3f StringToVec3(std::string Vector)
{
	if (Vector.empty())
		return Vec3f(0);

	std::string First, Second, Third;
	int Index = 0;

	for (char i : Vector)
	{
		if (i == '(' || i == ')')
			continue;

		if (i == ',')
		{
			Index++;
			continue;
		}

		switch (Index)
		{
		case 0:
			First.push_back(i);
			break;
		case 1:
			Second.push_back(i);
			break;
		case 2:
			Third.push_back(i);
			break;
		default:
			break;
		}
	}

	try
	{
		return Index ? Vec3f(std::stof(First), std::stof(Second), std::stof(Third)) : Vec3f(std::stof(First));
	}
	catch (std::exception e)
	{
		return 0;
	}
}

static Vec2f StringToVec2(std::string Vector)
{
	if (Vector.empty())
		return 0;

	std::string First, Second;
	bool IsSecond = false;

	for (char i : Vector)
	{
		if (i == '(' || i == ')')
			continue;

		if (i == ',')
		{
			IsSecond = true;
			continue;
		}

		if (IsSecond)
		{
			Second.push_back(i);
		}
		else
		{
			First.push_back(i);
		}
	}

	try
	{
		return IsSecond ? Vec2f(std::stof(First), std::stof(Second)) : Vec2f(std::stof(First));
	}
	catch (std::exception e)
	{
		return 0;
	}

}

static MarkupStructure::PropElementType GetTypeFromBox(UIBox* Target)
{
	using namespace MarkupStructure;

	const std::type_info& TypeId = typeid(*Target);


	if (TypeId == typeid(UIBlurBackground))
		return PropElementType::UIBlurBackground;

	if (TypeId == typeid(UIButton))
		return PropElementType::UIButton;

	if (TypeId == typeid(UITextField))
		return PropElementType::UITextField;

	if (TypeId == typeid(UIBackground))
		return PropElementType::UIBackground;

	if (TypeId == typeid(UIText))
		return PropElementType::UIText;

	if (TypeId == typeid(UIScrollBox))
		return PropElementType::UIScrollBox;

	return PropElementType::UIBox;
}

DynamicProperty* GetProperty(std::string Name, UIBox* Target, MarkupStructure::PropElementType Type)
{
	for (auto& i : DynamicProperties)
	{
		if (i.Name != Name)
			continue;

		if (!i.IsTarget(Target))
			continue;
		return &i;
	}
	return nullptr;
}

void kui::markup::SetUIBoxValue(UIBox* Target, std::string Name, std::string Value, DynamicMarkupContext* From, UIDynMarkupBox* Root, MarkupElement& Element)
{
	std::string InitialValue = Value;
	Constant* FoundConstant = From->Parsed->GetConstant(Value);
	Global* FoundGlobal = From->Parsed->GetGlobal(Value);
	auto MapVariable = Element.Root.Variables.find(Value);

	UIElement::Variable* FoundVariable = MapVariable != Element.Root.Variables.end() ? &MapVariable->second : nullptr;

	if (FoundConstant)
	{
		Value = FoundConstant->Value;
	}
	else if (FoundGlobal)
	{
		Root->Globals.insert({ Target,
			UIDynMarkupBox::VariableValue{
				.Box = Target,
				.Name = Name,
				.Value = InitialValue,
			} });
		Value = FoundGlobal->Value;
	}
	else if (FoundVariable)
	{

		Root->Variables[FoundVariable->Token.Text].Elements.insert({ Target,
			UIDynMarkupBox::VariableValue{
				.Box = Target,
				.Name = Name,
				.Value = InitialValue,
			} });
		Value = FoundVariable->Value;
	}

	auto Type = GetTypeFromBox(Target);

	for (auto& i : MarkupStructure::Properties)
	{
		if (i.Name != Name)
			continue;

		if (Type != i.Type && !IsSubclassOf(Type, i.Type))
			continue;

		auto Found = GetProperty(Name, Target, Type);

		if (!Found)
			continue;

		auto ValueAny = ToAny(Value, [Root, Target, Name, InitialValue](std::string InValue)
			{
				auto& Entry = Root->Translated[Target];
				Entry.Box = Target;
				Entry.Name = Name;
				Entry.Value = InitialValue;
				std::string Translated = EscapeString(InValue.substr(1));
				return Root->GetTranslation(Translated.c_str());
			}, i.VarType);

		if (FoundGlobal)
		{
			ValueAny = Root->GetGlobal(FoundGlobal->Name.Text.c_str(), ValueAny);
		}

		if (FoundVariable)
		{
			auto& Var = Root->Variables[FoundVariable->Token.Text];
			if (Var.Value.Empty)
				Var.Value = ValueAny;
			else
				ValueAny = Var.Value;
		}

		try
		{
			Found->SetValue(Target, ValueAny);
		}
		catch (std::bad_any_cast& ce)
		{
			std::cerr << ce.what() << std::endl;
		}
		return;
	}


	UIDynMarkupBox* ChildMarkupBox = dynamic_cast<UIDynMarkupBox*>(Target);
	if (!ChildMarkupBox || !ChildMarkupBox->Element)
	{
		return;
	}
	for (auto& i : ChildMarkupBox->Element->Root.Variables)
	{
		if (Name == i.first)
		{
			auto ValueAny = ToAny(Value, [Root, Target, Name, InitialValue](std::string InValue)
				{
					auto& Entry = Root->Translated[Target];
					Entry.Box = Target;
					Entry.Name = Name;
					Entry.Value = InitialValue;
					std::string Translated = EscapeString(InValue.substr(1));
					return Root->GetTranslation(Translated.c_str());
				}, i.second.Type);

			if (FoundGlobal)
			{
				ValueAny = Root->GetGlobal(FoundGlobal->Name.Text.c_str(), ValueAny);
			}
			ChildMarkupBox->SetVariable(Name, ValueAny);
		}
	}
}

kui::AnyContainer kui::markup::ToAny(std::string Value, std::function<std::string(std::string)> GetTranslated, MarkupStructure::VariableType Type)
{
	kui::AnyContainer ValueAny;
	switch (Type)
	{
	case  VariableType::Number:
		ValueAny = std::stof(Value);
		break;
	case VariableType::SizeNumber:
	{
		auto Size = stringParse::Size(Value, true);
		if (Size.SizeValue.empty())
			ValueAny = UISize::Smallest();
		else
			ValueAny = UISize(std::stof(Size.SizeValue), ConvertSizeMode(Size.SizeMode));
		break;
	}
	case VariableType::Size:
	{
		auto Size = stringParse::Size(Value);
		ValueAny = SizeVec(StringToVec2(Size.SizeValue), ConvertSizeMode(Size.SizeMode));
		break;
	}
	case VariableType::Vec2:
	{
		ValueAny = StringToVec2(Value);
		break;
	}
	case VariableType::Vector3:
	{
		ValueAny = StringToVec3(Value);
		break;
	}
	case VariableType::String:
		if (Value.size() && Value[0] == '$')
			Value = GetTranslated(Value);
		else
			Value = EscapeString(Value);
		[[fallthrough]];
	case VariableType::Orientation:
	case VariableType::Align:
	{
		ValueAny = Value;
		break;
	}
	default:
		break;
	}

	return ValueAny;
}
