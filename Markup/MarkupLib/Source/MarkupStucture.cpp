#include <Markup/MarkupStructure.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Markup/Format.h"
#include "Markup/StringParse.h"
#include <sstream>
#include <map>
#include <Markup/ParseError.h>
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
	// If the VarType is Size, then a second function needs to be specified that sets the size mode.
	std::string SetSizeFormat;
	std::string (*CreateCodeFunction)(std::string InValue) = nullptr;
	UIElement::Variable::VariableType VarType;
	bool AlwaysSet = false;
	std::string Default;
};

std::map<UIElement::Variable::VariableType, UIElement::Variable::VariableTypeDescription> UIElement::Variable::Descriptions =
{
{
	VariableType::None,
	VariableTypeDescription("No type", "KlemmUI::AnyContainer"),
},
{
	VariableType::Number,
	VariableTypeDescription("Number", "float"),
},
{
	VariableType::String,
	VariableTypeDescription("String", "std::string"),
},
{
	VariableType::Size,
	VariableTypeDescription("Size", "Vector2f"),
},
{
	VariableType::SizeMode,
	VariableTypeDescription("SizeMode", "KlemmUI::UIBox::SizeMode"),
},
{
	VariableType::Align,
	VariableTypeDescription("Align", "KlemmUI::UIBox::Align"),
},
{
	VariableType::Vector3,
	VariableTypeDescription("Vector3", "Vector3f"),
},
{
	VariableType::Vector2,
	VariableTypeDescription("Vector2", "Vector2f"),
},
{
	VariableType::Bool,
	VariableTypeDescription("Boolean", "bool"),
},
{
	VariableType::Orientation,
	VariableTypeDescription("Orientation", "bool"),
},
{
	VariableType::Callback,
	VariableTypeDescription("Callback", ""),
},
{
	VariableType::CallbackIndex,
	VariableTypeDescription("Callback with index", ""),
},
};

static std::vector<PropertyElement> Properties
{
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "text",
		.SetFormat = {"SetText({val})"},
		.VarType = UIElement::Variable::VariableType::String,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "sizeMode",
		.SetFormat = {"SetTextSizeMode({val})"},
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "size",
		.SetFormat = {"SetTextSize({val})"},
		.SetSizeFormat = "SetTextSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "font",
		.SetFormat = {"SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont({val}))"},
		.VarType = UIElement::Variable::VariableType::String,
		.AlwaysSet = true,
		.Default = "\"\""
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))", "SetHoveredColor(Vector3f({val}) * 0.75f)", "SetPressedColor(Vector3f({val}) * 0.5f)"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "opacity",
		.SetFormat = {"SetOpacity({val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "onClicked",
		.SetFormat = {"OnClickedFunction = {val}"},
		.VarType = UIElement::Variable::VariableType::Callback,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "minSize",
		.SetFormat = {"SetMinSize({val})"},
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "size",
		.SetFormat = {"SetMinSize(Vector2f({val}))", "SetMaxSize(Vector2f({val}))"},
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "position",
		.SetFormat = {"SetPosition(Vector2f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector2,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "allAlign",
		.SetFormat = {"SetVerticalAlign(KlemmUI::UIBox::Align::{val})", "SetHorizontalAlign(KlemmUI::UIBox::Align::{val})"},
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "horizontalAlign",
		.SetFormat = {"SetHorizontalAlign(KlemmUI::UIBox::Align::{val})"},
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "verticalAlign",
		.SetFormat = {"SetVerticalAlign(KlemmUI::UIBox::Align::{val})"},
			.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "paddingSizeMode",
		.SetFormat = {"SetPaddingSizeMode({val})"},
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "sizeMode",
		.SetFormat = {"SetSizeMode({val})"},
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "padding",
		.SetFormat = {"SetPadding((float){val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "upPadding",
		.SetFormat = {"SetUpPadding((float){val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "downPadding",
		.SetFormat = {"SetDownPadding((float){val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "leftPadding",
		.SetFormat = {"SetLeftPadding((float){val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "rightPadding",
		.SetFormat = {"SetRightPadding((float){val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "hasMouseCollision",
		.SetFormat = {"HasMouseCollision((bool){val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "tryFill",
		.SetFormat = {"SetTryFill((bool){val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "orientation",
		.CreateCodeFunction = [](std::string Val) -> std::string {
			if (Val == "Horizontal")
			{
				return "SetHorizontal(true)";
			}
			if (Val == "Vertical")
			{
				return "SetHorizontal(false)";
			}
			return "";
		},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
};

static std::map<PropElementType, std::string> DefaultConstructors =
{
	{PropElementType::UIBox, "true"},
	{PropElementType::UIText, "1, 1, \"\", nullptr"},
	{PropElementType::UIButton, "true, 0, 1, nullptr"},
};

void MarkupElement::WriteHeader(const std::string& Path, std::vector<MarkupElement>& MarkupElements)
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

	Out << "class " << Root.TypeName << " : public KlemmUI::UIBox\n{\n";

	std::string Constructor = MakeConstructor(MarkupElements);
	
	for (auto& i : Root.Variables)
	{
		std::string TypeName = UIElement::Variable::Descriptions[i.second.Type].CppName;
		Out << "\t" << TypeName << " " << i.first;
		if (!i.second.Value.empty())
		{
			std::string Value = StringParse::ToCppCode(i.second.Value);

			Out << " = " << TypeName << "(" << Value << ")";
		}
		Out << ";\n";
	}
	Out << "\npublic:\n";


	auto NamedElements = Root.GetNamedElements();
	for (auto& i : NamedElements)
	{
		Out << "\t" << i << ";" << std::endl;
	}

	Out << Constructor << std::endl;

	for (auto& i : Root.Variables)
	{
		Out << "\t" << Root.WriteVariableSetter(i);
	}

	Out << "};" << std::endl;
	Out.close();
}

std::string MarkupElement::MakeConstructor(std::vector<MarkupElement>& MarkupElements)
{
	std::stringstream OutStream;

	OutStream << "\t" << Root.TypeName << "() : UIBox(true)\n\t{\n";
	OutStream << Root.MakeCode("", &Root, 0, MarkupElements);
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
static int UnnamedCounter = 0;

static std::string WriteElementProperty(UIElement* Target, UIElement* Root, std::string ElementName, Property p, const PropertyElement& i)
{
	auto Variable = Root->Variables.find(p.Value);

	if (Variable != Root->Variables.end())
	{
		if (Variable->second.Type != i.VarType)
		{
			if (Variable->second.Type == UIElement::Variable::VariableType::None)
			{
				Variable->second.Type = i.VarType;
			}
			else
			{
				KlemmUI::ParseError::ErrorNoLine("Variable '" + Variable->first + "' has a type mismatch.");
			}
		}
		if (Target->ElementName.empty())
		{
			Target->ElementName = "unnamed_" + std::to_string(UnnamedCounter);
		}
	}
	else if (i.VarType == UIElement::Variable::VariableType::String && !KlemmUI::StringParse::IsStringToken(p.Value))
	{
		KlemmUI::ParseError::ErrorNoLine("Expected a string for value '" + i.Name + "'");
	}
	else if (i.VarType == UIElement::Variable::VariableType::Size && !KlemmUI::StringParse::IsSizeValue(p.Value))
	{
		KlemmUI::ParseError::ErrorNoLine("Expected a size for value '" + i.Name + "'");
	}
	else if (i.VarType == UIElement::Variable::VariableType::SizeMode)
	{
		p.Value = KlemmUI::StringParse::Size::SizeModeToKUISizeMode(p.Value);
	}


	if (i.CreateCodeFunction)
	{
		return "\t" + ElementName + "->" + i.CreateCodeFunction(p.Value) + ";\n";
	}

	std::string Value;

	for (auto& FormatString : i.SetFormat)
	{
		std::string Format = "->" + Format::FormatString(FormatString, { Format::FormatArg("val", KlemmUI::StringParse::ToCppCode(p.Value)) });

		Value += "\t" + ElementName + Format + ";\n";
		if (Variable != Root->Variables.end())
		{
			Variable->second.References.push_back(Target->ElementName + Format);
		}
	}

	if (i.VarType == UIElement::Variable::VariableType::Size)
	{
		auto val = KlemmUI::StringParse::Size(p.Value);

		if (!val.SizeMode.empty())
		{
			std::string Format = ElementName + "->" + Format::FormatString(i.SetSizeFormat, {
				Format::FormatArg("val", KlemmUI::StringParse::Size::SizeModeToKUISizeMode(val.SizeMode))
				});
			Value += "\t" + Format + ";\n";
		}
	}

	return Value;
}

std::string UIElement::MakeCode(std::string Parent, UIElement* Root, size_t Depth, std::vector<MarkupElement>& MarkupElements)
{
	bool HasName = !ElementName.empty();
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

	for (auto& elem : MarkupElements)
	{
		if (elem.Root.TypeName != TypeName)
		{
			continue;
		}
		for (auto& prop : PropertyCopy)
		{
			if (!elem.Root.Variables.contains(prop.Name))
			{
				continue;
			}
			OutStream << "\t" << ElementName << "->Set" << prop.Name << "(" << prop.Value << ");" << std::endl;
		}
	}

	if (!Parent.empty())
	{
		OutStream << "\t" << Parent << "->AddChild(" << ElemName << ");" << std::endl;
	}

	// Name was set because a variable setter uses this element.
	if (!HasName && !ElementName.empty())
	{
		// Assign the local variable of the generated element to the global one so it can be used in the setters.
		OutStream << "\t" << ElementName << " = " << ElemName << ";" << std::endl;
	}

	if (!Children.empty())
	{
		OutStream << "\t{\n";
		for (auto& i : Children)
		{
			OutStream << i.MakeCode(ElemName, Root, Depth + 1, MarkupElements);
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

	Out << "void Set" << Var.first << "(" << Variable::Descriptions[Var.second.Type].CppName << " NewValue)\n\t{" << std::endl;
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
	Deps.insert("KlemmUI/Markup/Markup.h");
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
			std::cout << "FOUND: " << i.ElementName << std::endl;
		}
		auto ElemDeps = i.GetNamedElements();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	return Deps;
}

