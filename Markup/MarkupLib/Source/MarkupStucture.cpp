#include <Markup/MarkupStructure.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Markup/Format.h"
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
};

static std::vector<PropertyElement> Properties
{
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "text",
		.SetFormat = {"SetText(\"{val}\")"}
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
		.SetFormat = {"SetFont(KlemmUI::MarkupElement::GetFont(\"{val}\"))"},
		.AlwaysSet = true
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "minSize",
		.SetFormat = {"SetMinSize({val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "size",
		.SetFormat = {"SetMinSize({val})", "SetMaxSize({val})"}
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "position",
		.SetFormat = {"SetPosition({val})"}
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

	Out << "class " << Root.TypeName << "\n{\npublic:\n\t";
	Out << MakeConstructor() << std::endl;
	Out << "};" << std::endl;
	Out.close();
}

std::string MarkupElement::MakeConstructor() const
{
	std::stringstream OutStream;

	OutStream << Root.TypeName << "()\n\t{\n";
	OutStream << Root.MakeCode("", 0);
	OutStream << "\n\t}" << std::endl;

	return OutStream.str();
}

PropElementType GetTypeFromString(std::string TypeName)
{
	PropElementType ElemType = PropElementType::UIBox;
	if (TypeName == "UIText")
	{
		ElemType = PropElementType::UIText;
	}
	if (TypeName == "UIBackground")
	{
		ElemType = PropElementType::UIBackground;
	}
	return ElemType;
}

std::string UIElement::MakeCode(std::string Parent, size_t Depth) const
{

	std::stringstream OutStream;
	std::string ElemName = "e_" + std::to_string(Depth);
	if (Type == ElementType::Default)
	{
		OutStream << "\tauto* " << ElemName << " = new KlemmUI::" << TypeName << "(" << DefaultConstructors[GetTypeFromString(TypeName)] << ");\n";

		for (auto& i : ElementProperties)
		{
			OutStream << WriteElementProperty(ElemName, i);
		}
		if (!Parent.empty())
		{
			OutStream << "\t" << Parent << "->AddChild(" << ElemName << ");\n" << std::endl;
		}
	}
	else
	{
		ElemName.clear();
	}

	if (!Children.empty())
	{
		OutStream << "\t{\n";
		for (auto& i : Children)
		{
			OutStream << i.MakeCode(ElemName, Depth + 1);
		}
		OutStream << "\t}\n";
	}
	return OutStream.str();
}

std::string KlemmUI::MarkupStructure::UIElement::WriteElementProperty(std::string ElementName, const Property& p) const
{
	for (auto& i : Properties)
	{
		if (IsSubclassOf(GetTypeFromString(TypeName), i.Type) && i.Name == p.Name)
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
					Value += "\t" + ElementName + "->" + Format::FormatString(FormatString, {Format::FormatArg("val", p.Value)}) + ";\n";
				}
				return Value;
			}
		}
	}
	return "";
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
			Deps.insert(i.ElementName + ".kui.hpp");
		}
		auto ElemDeps = i.GetElementDependencies();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	return Deps;
}

