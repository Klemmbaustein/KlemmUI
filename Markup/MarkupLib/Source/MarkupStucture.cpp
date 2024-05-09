#include <Markup/MarkupStructure.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Markup/Format.h"
#include "Markup/StringParse.h"
#include <sstream>
#include <map>
using namespace KlemmUI::MarkupStructure;

enum class PropElementType
{
	UIBox,
	Backgrounds_Begin,
	UIBackground,
	UIButton,
	Backgrounds_End,
	UIText,
	Unknown,
};

static bool IsSubclassOf(PropElementType Class, PropElementType Parent)
{
	if (Parent == PropElementType::UIBox)
	{
		return true;
	}

	int ClassVal = (int)Class;

	if (Parent == PropElementType::UIBackground
		&& ClassVal > (int)PropElementType::Backgrounds_Begin
		&& ClassVal < (int)PropElementType::Backgrounds_End)
	{
		return true;
	}

	return Class == Parent;
}

struct PropertyElement
{
	PropElementType Type;
	std::string Name;
	std::vector<std::string> SetFormat;
	std::string (*CreateCodeFunction)(std::string InValue) = nullptr;
	bool AlwaysSet = false;
	std::string Default;
};

static std::vector<PropertyElement> Properties
{
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "text",
		.SetFormat = {"SetText({val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "sizeMode",
		.SetFormat = {"SetTextSizeMode(KlemmUI::UIBox::SizeMode::{val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "size",
		.SetFormat = {"SetTextSize({val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "font",
		.SetFormat = {"SetFont(KlemmUI::MarkupElement::GetFont({val}))"},
		.AlwaysSet = true,
		.Default = "\"\""
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))", "SetHoveredColor(Vector3f({val}) * 0.75f)", "SetPressedColor(Vector3f({val}) * 0.5f)"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "minSize",
		.SetFormat = {"SetMinSize({val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "size",
		.SetFormat = {"SetMinSize(Vector2f({val}))", "SetMaxSize(Vector2f({val}))"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "position",
		.SetFormat = {"SetPosition(Vector2f({val}))"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "allAlign",
		.SetFormat = {"SetVerticalAlign(KlemmUI::UIBox::Align::{val})", "SetHorizontalAlign(KlemmUI::UIBox::Align::{val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "paddingSizeMode",
		.SetFormat = {"SetPaddingSizeMode(KlemmUI::UIBox::SizeMode::{val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "sizeMode",
		.SetFormat = {"SetSizeMode(KlemmUI::UIBox::SizeMode::{val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "padding",
		.SetFormat = {"SetPadding((float){val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "orientation",
		.CreateCodeFunction = [](std::string Val) -> std::string {
			if (Val == "horizontal")
			{
				return "SetIsHorizontal(true)";
			}
			if (Val == "vertical")
			{
				return "SetIsHorizontal(false)";
			}
			return "";
		}
	},
};

static std::map<PropElementType, std::string> DefaultConstructors =
{
	{PropElementType::UIBox, "true"},
	{PropElementType::UIText, "1, 1, \"\", nullptr"},
	{PropElementType::UIButton, "true, 0, 1, nullptr"},
};

void MarkupElement::WriteHeader(const std::string& Path)
{
	std::filesystem::create_directories(Path);
	std::string ElementPath = Path + "/" + Root.TypeName + ".hpp";
	std::ofstream Out = std::ofstream(ElementPath);
	Out << "#pragma once" << std::endl;

	auto Dependencies = Root.GetElementDependencies();

	for (auto& i : Dependencies)
	{
		Out << "#include \"" << i << "\"" << std::endl;
	}

	Out << "class " << Root.TypeName << " : public KlemmUI::UIBox\n{\npublic:\n";

	auto NamedElements = Root.GetNamedElements();
	for (auto& i : NamedElements)
	{
		Out << "\t" << i << ";" << std::endl;
	}
	Out << MakeConstructor() << std::endl;

	for (auto& i : Root.Variables)
	{
		Out << "\t" << Root.WriteVariableSetter(i);
	}

	Out << "protected:\n";
	for (auto& i : Root.Variables)
	{
		Out << "\tKlemmUI::AnyContainer " << i.first;
		if (!i.second.Value.empty())
		{
			Out << " = " << i.second.Value;
		}
		Out << ";\n";
	}

	Out << "};" << std::endl;
	Out.close();
}

std::string MarkupElement::MakeConstructor()
{
	std::stringstream OutStream;

	OutStream << "\t" << Root.TypeName << "() : UIBox(true)\n\t{\n";
	OutStream << Root.MakeCode("", &Root, 0);
	OutStream << "\n\t}" << std::endl;

	return OutStream.str();
}

static PropElementType GetTypeFromString(std::string TypeName)
{
	if (TypeName == "UIBox")
	{
		return PropElementType::UIBox;
	}
	if (TypeName == "UIText")
	{
		return PropElementType::UIText;
	}
	if (TypeName == "UIBackground")
	{
		return PropElementType::UIBackground;
	}
	if (TypeName == "UIButton")
	{
		return PropElementType::UIButton;
	}
	return PropElementType::Unknown;
}

static std::string WriteElementProperty(UIElement* Target, UIElement* Root, std::string ElementName, const Property& p, const PropertyElement& i)
{
	if (i.CreateCodeFunction)
	{
		return "\t" + ElementName + "->" + i.CreateCodeFunction(p.Value) + ";\n";
	}
	else
	{
		std::string Value;

		for (auto& FormatString : i.SetFormat)
		{
			std::string Format = ElementName + "->" + Format::FormatString(FormatString, { Format::FormatArg("val", KlemmUI::StringParse::ToCppCode(p.Value)) });

			Value += "\t" + Format + ";\n";
			if (Root->Variables.contains(p.Value))
			{
				Root->Variables.at(p.Value).References.push_back(Format);
			}
		}
		return Value;
	}
	return "";
}

std::string UIElement::MakeCode(std::string Parent, UIElement* Root, size_t Depth)
{
	std::stringstream OutStream;
	std::string ElemName = ElementName.empty() ? ("e_" + std::to_string(Depth)) : ElementName;
	if (!Parent.empty())
	{
		OutStream << "\t";
		if (ElementName.empty())
		{
			OutStream << "auto* ";
		}
		std::string ClassName = TypeName;
		if (Type == ElementType::Default)
		{
			ClassName = "KlemmUI::" + ClassName;
		}
		OutStream << ElemName << " = new " << ClassName << "(" << DefaultConstructors[GetTypeFromString(TypeName)] << ");\n";
	}
	else
	{
		ElemName = "this";
	}
	std::vector PropertyCopy = ElementProperties;
	for (auto& Prop : Properties)
	{
		if (!IsSubclassOf(GetTypeFromString(TypeName), Prop.Type))
		{
			continue;
		}
		bool Found = false;
		for (auto& i : PropertyCopy)
		{
			if (i.Name == Prop.Name)
			{
				OutStream << WriteElementProperty(this, Root, ElemName, i, Prop);
				i.Name.clear();
				Found = true;
				break;
			}
		}
		if (!Found && Prop.AlwaysSet)
		{
			OutStream << WriteElementProperty(this, Root, ElemName, Property{
				.Name = Prop.Name,
				.Value = Prop.Default
				}, Prop);
		}
	}
	if (!Parent.empty())
	{
		OutStream << "\t" << Parent << "->AddChild(" << ElemName << ");\n" << std::endl;
	}

	if (!Children.empty())
	{
		OutStream << "\t{\n";
		for (auto& i : Children)
		{
			OutStream << i.MakeCode(ElemName, Root, Depth + 1);
		}
		OutStream << "\t}\n";
	}
	return OutStream.str();
}

bool KlemmUI::MarkupStructure::UIElement::IsDefaultElement(const std::string& Name)
{
	return GetTypeFromString(Name) != PropElementType::Unknown;
}

std::string KlemmUI::MarkupStructure::UIElement::WriteVariableSetter(std::pair<std::string, Variable> Var)
{
	std::stringstream Out;

	Out << "void Set" << Var.first << "(KlemmUI::AnyContainer NewValue)\n\t{" << std::endl;
	Out << "\t\t" << Var.first << " = NewValue;\n";
	for (auto& i : Var.second.References)
	{
		Out << "\t\t" << Format::FormatString(i, { Format::FormatArg("val", "NewValue") }) << ";" << std::endl;
	}
	Out << "\t}\n";

	return Out.str();
}

std::set<std::string> UIElement::GetElementDependencies() const
{
	std::set<std::string> Deps;
	for (auto& i : Children)
	{
		if (i.Type == ElementType::Default)
		{
			Deps.insert("KlemmUI/UI/" + i.TypeName + ".h");
		}
		else
		{
			Deps.insert(i.TypeName + ".hpp");
		}
		auto ElemDeps = i.GetElementDependencies();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	Deps.insert("KlemmUI/UI/UIBox.h");
	return Deps;
}

std::set<std::string> KlemmUI::MarkupStructure::UIElement::GetNamedElements() const
{
	std::set<std::string> Deps;
	for (auto& i : Children)
	{
		if (!i.ElementName.empty())
		{
			std::string Out = i.TypeName + "* " + i.ElementName;
			if (IsDefaultElement(i.TypeName))
			{
				Out = "KlemmUI::" + Out;
			}
			Deps.insert(Out);
		}
		auto ElemDeps = i.GetNamedElements();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	return Deps;
}

