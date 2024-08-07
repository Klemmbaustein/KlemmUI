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
	VariableType::SizeNumber,
	VariableTypeDescription("1d Size", "float"),
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
	VariableType::BorderType,
	VariableTypeDescription("Border type", "KlemmUI::UIBox::BorderType"),
},
{
	VariableType::CallbackIndex,
	VariableTypeDescription("Callback with index", ""),
},
};

#include "ElementProperties.h"

static std::map<PropElementType, std::string> DefaultConstructors =
{
	{PropElementType::UIBox, "true"},
	{PropElementType::UIText, "1, 1, \"\", nullptr"},
	{PropElementType::UIButton, "true, 0, 1, nullptr"},
	{PropElementType::UIBackground, "true, 0, 1"},
};

static int UnnamedCounter = 0;
void MarkupElement::WriteHeader(const std::string& Path, ParseResult& MarkupElements)
{
	std::vector<KlemmUI::StringParse::Line> Lines = {};
	ParseError::SetCode(Lines, File);
	UnnamedCounter = 0;
	std::filesystem::create_directories(Path);
	std::string ElementPath = Path + "/" + Root.TypeName + ".hpp";
	std::stringstream Out;
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
			Constant* ValueConstant = MarkupElements.GetConstant(i.second.Value);

			if (ValueConstant)
			{
				i.second.Value = ValueConstant->Value;
			}

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


	bool ShouldWrite = true;
	if (std::filesystem::exists(ElementPath))
	{
		std::ifstream In = std::ifstream(ElementPath);
		std::string OldContent = std::string(std::istreambuf_iterator<char>(In.rdbuf()),
			std::istreambuf_iterator<char>());
		In.close();
		ShouldWrite = Out.str() != OldContent;
	}
	if (ShouldWrite)
	{
		std::ofstream OutFile = std::ofstream(ElementPath);
		OutFile << Out.rdbuf();
		OutFile.close();
	}
}

std::string MarkupElement::MakeConstructor(ParseResult& MarkupElements)
{
	std::stringstream OutStream;

	OutStream << "\t" << Root.TypeName << "() : UIBox(true)\n\t{\n";
	size_t Depth = 0;
	OutStream << Root.MakeCode("", &Root, Depth, MarkupElements);
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

Constant* ParseResult::GetConstant(std::string Name)
{
	for (Constant& i : Constants)
	{
		if (i.Name == Name)
		{
			return &i;
		}
	}
	return nullptr;
}

static std::string WriteElementProperty(UIElement* Target, UIElement* Root, std::string ElementName, Property p, const PropertyElement& i, ParseResult& MarkupElements)
{
	auto Variable = Root->Variables.find(p.Value);

	Constant* ValueConstant = MarkupElements.GetConstant(p.Value);

	if (ValueConstant)
	{
		p.Value = ValueConstant->Value;
	}

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
				KlemmUI::ParseError::ErrorNoLine("Variable '" + Variable->first + "' does not have the correct type for the value of '" + i.Name + "'");
			}
		}
		if (Target->ElementName.empty())
		{
			Target->ElementName = "unnamed_" + std::to_string(UnnamedCounter++);
		}
	}
	else
	{
		switch (i.VarType)
		{
		case UIElement::Variable::VariableType::String:
			if (!KlemmUI::StringParse::IsStringToken(p.Value))
				KlemmUI::ParseError::ErrorNoLine("Expected a string for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::Size:
			if (!KlemmUI::StringParse::IsSizeValue(p.Value))
				KlemmUI::ParseError::ErrorNoLine("Expected a size for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::SizeNumber:
			if (!KlemmUI::StringParse::Is1DSizeValue(p.Value))
				KlemmUI::ParseError::ErrorNoLine("Expected a size for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::Align:
			if (KlemmUI::StringParse::GetAlign(p.Value).empty())
				KlemmUI::ParseError::ErrorNoLine("Expected a valid align value for '" + i.Name + "'");
			p.Value = KlemmUI::StringParse::GetAlign(p.Value);
			break;

		case UIElement::Variable::VariableType::BorderType:
			if (KlemmUI::StringParse::GetBorderType(p.Value).empty())
				KlemmUI::ParseError::ErrorNoLine("Expected a valid border type value for '" + i.Name + "'");
			p.Value = KlemmUI::StringParse::GetBorderType(p.Value);
			break;

		case UIElement::Variable::VariableType::SizeMode:
			p.Value = KlemmUI::StringParse::Size::SizeModeToKUISizeMode(p.Value);
			break;

		case UIElement::Variable::VariableType::Vector3:
		case UIElement::Variable::VariableType::Vector2:
			if (!KlemmUI::StringParse::IsVectorToken(p.Value) || KlemmUI::StringParse::IsNumber(p.Value))
			{
				p.Value = "(" + p.Value + ")";
			}
			break;
		default:
			break;
		}
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

	if (i.VarType == UIElement::Variable::VariableType::Size || i.VarType == UIElement::Variable::VariableType::SizeNumber)
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

std::string UIElement::MakeCode(std::string Parent, UIElement* Root, size_t& Depth, ParseResult& MarkupElements)
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
				OutStream << WriteElementProperty(this, Root, ElemName, i, Prop, MarkupElements);
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
				}, Prop, MarkupElements);
		}
	}

	for (auto& elem : MarkupElements.Elements)
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

			Constant* ValueConstant = MarkupElements.GetConstant(prop.Value);

			if (ValueConstant)
			{
				prop.Value = ValueConstant->Value;
			}

			OutStream << "\t" << ElemName << "->Set" << prop.Name << "(" << KlemmUI::StringParse::ToCppCode(prop.Value) << ");" << std::endl;
			prop.Name.clear();
		}
	}

	for (auto& i : PropertyCopy)
	{
		if (i.Name.empty())
		{
			continue;
		}
		ParseError::ErrorNoLine("Unknown property: " + i.Name);
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
			OutStream << i.MakeCode(ElemName, Root, ++Depth, MarkupElements);
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
		}
		auto ElemDeps = i.GetNamedElements();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	return Deps;
}

