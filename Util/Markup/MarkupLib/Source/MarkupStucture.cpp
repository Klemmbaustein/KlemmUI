#include <Markup/MarkupStructure.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Markup/Format.h"
#include "Markup/StringParse.h"
#include <sstream>
#include <map>
#include <Markup/ParseError.h>
using namespace kui::MarkupStructure;

bool kui::MarkupStructure::IsSubclassOf(PropElementType Class, PropElementType Parent)
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


std::map<UIElement::Variable::VariableType, UIElement::Variable::VariableTypeDescription> UIElement::Variable::Descriptions =
{
{
	VariableType::None,
	VariableTypeDescription("No type", "kui::AnyContainer"),
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
	VariableTypeDescription("SizeVector", "Vec2f"),
},
{
	VariableType::SizeNumber,
	VariableTypeDescription("SizeValue", "float"),
},
{
	VariableType::SizeMode,
	VariableTypeDescription("SizeMode", "kui::UIBox::SizeMode"),
},
{
	VariableType::Align,
	VariableTypeDescription("Align", "kui::UIBox::Align"),
},
{
	VariableType::Vector3,
	VariableTypeDescription("Vector3", "Vec3f"),
},
{
	VariableType::Vec2,
	VariableTypeDescription("Vec2", "Vec2f"),
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
	VariableTypeDescription("Callback", "std::function<void()>"),
},
{
	VariableType::CallbackIndex,
	VariableTypeDescription("Callback with index", "std::function<void(int)>"),
},
};

#include "ElementProperties.h"

static std::map<PropElementType, std::string> DefaultConstructors =
{
	{PropElementType::UIBox, "true"},
	{PropElementType::UIText, "1, 1, \"\", nullptr"},
	{PropElementType::UIButton, "true, 0, 1, nullptr"},
	{PropElementType::UIBackground, "true, 0, 1"},
	{PropElementType::UITextField, "0, 1, nullptr, nullptr"},
	{PropElementType::UIScrollBox, "true, 0, true"},
};

static int UnnamedCounter = 0;
std::string MarkupElement::WriteCode(ParseResult& MarkupElements)
{
	std::vector<kui::stringParse::Line> Lines = {};
	parseError::SetCode(Lines, File);
	UnnamedCounter = 0;
	std::stringstream Out;

	std::string Constructor = WriteLayoutFunction(MarkupElements);

	auto Dependencies = Root.GetElementDependencies();

	for (auto& i : Dependencies)
	{
		Out << "#include \"" << i << "\"" << std::endl;
	}

	Out << "class " << Root.TypeName.Text << " : public kui::UIBox, kui::markup::MarkupBox\n{\n";

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

			std::string Value = stringParse::ToCppCode(i.second.Value);

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

	Out << "\tvirtual void OnTranslationChanged() override\n\t{\n";
	for (auto& i : Root.TranslatedProperties)
	{
		Out << "\t\t" << Format::FormatString(i.Name, { Format::FormatArg("val", kui::stringParse::ToCppCode(i.Value)) }) << ";\n";
	}
	Out << "\t}";

	Out << "};\n";
	return Out.str();
}

std::string MarkupElement::WriteLayoutFunction(ParseResult& MarkupElements)
{
	std::stringstream OutStream;

	OutStream << "\t" << Root.TypeName.Text << "() : UIBox(true)\n\t{\n";
	size_t Depth = 0;
	OutStream << Root.MakeCode("", &Root, Depth, MarkupElements);
	OutStream << "\n\t}" << std::endl;

	return OutStream.str();
}

PropElementType kui::MarkupStructure::GetTypeFromString(std::string TypeName)
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
	if (TypeName == "UITextField")
	{
		return PropElementType::UITextField;
	}
	if (TypeName == "UIScrollBox")
	{
		return PropElementType::UIScrollBox;
	}
	return PropElementType::Unknown;
}

std::string kui::MarkupStructure::GetStringFromType(PropElementType Type)
{
	if (Type == PropElementType::UIBox)
	{
		return "UIBox";
	}
	if (Type == PropElementType::UIText)
	{
		return "UIText";
	}
	if (Type == PropElementType::UIBackground)
	{
		return "UIBackground";
	}
	if (Type == PropElementType::UIButton)
	{
		return "UIButton";
	}
	if (Type == PropElementType::UITextField)
	{
		return "UITextField";
	}
	if (Type == PropElementType::UIScrollBox)
	{
		return "UIScrollBox";
	}
	return "Unknown";
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
	using namespace kui::stringParse;
	auto Variable = Root->Variables.find(p.Value);

	Constant* ValueConstant = MarkupElements.GetConstant(p.Value);

	if (ValueConstant)
	{
		p.Value = ValueConstant->Value;
	}

	bool IsTranslated = IsTranslatedString(p.Value) && i.VarType == UIElement::Variable::VariableType::String;

	if (Variable != Root->Variables.end() || IsTranslated)
	{
		if (!IsTranslated && Variable->second.Type != i.VarType)
		{
			if (Variable->second.Type == UIElement::Variable::VariableType::None)
			{
				Variable->second.Type = i.VarType;
			}
			else
			{
				kui::parseError::ErrorNoLine("Variable '" + Variable->first + "' does not have the correct type for the value of '" + i.Name + "'");
			}
		}
		if (Target->ElementName.Empty())
		{
			Target->ElementName = StringToken("unnamed_" + std::to_string(UnnamedCounter++), 0, 0);
		}
		if (IsTranslated)
		{
			Root->TranslatedProperties.push_back(Property(StringToken(Target->ElementName.Text + "->" + i.SetFormat[0], 0, 0), p.Value));
		}
	}
	else
	{

		switch (i.VarType)
		{
		case UIElement::Variable::VariableType::String:
			if (!IsStringToken(p.Value))
				kui::parseError::ErrorNoLine("Expected a string for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::Size:
			if (!IsSizeValue(p.Value))
				kui::parseError::ErrorNoLine("Expected a size for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::SizeNumber:
			if (!Is1DSizeValue(p.Value))
				kui::parseError::ErrorNoLine("Expected a size for value '" + i.Name + "'");
			break;

		case UIElement::Variable::VariableType::Align:
			if (GetAlign(p.Value).empty())
				kui::parseError::ErrorNoLine("Expected a valid align value for '" + i.Name + "'");
			p.Value = GetAlign(p.Value);
			break;

		case UIElement::Variable::VariableType::SizeMode:
			p.Value = Size::SizeModeToKUISizeMode(p.Value);
			break;

		case UIElement::Variable::VariableType::Vector3:
		case UIElement::Variable::VariableType::Vec2:
			if (!IsVectorToken(p.Value) || IsNumber(p.Value))
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
		std::string Format = "->" + Format::FormatString(FormatString, { Format::FormatArg("val", kui::stringParse::ToCppCode(p.Value)) });

		Value += "\t" + ElementName + Format + ";\n";
		if (Variable != Root->Variables.end())
		{
			Variable->second.References.push_back(Target->ElementName.Text + Format);
		}
	}

	if (i.VarType == UIElement::Variable::VariableType::Size || i.VarType == UIElement::Variable::VariableType::SizeNumber)
	{
		auto val = Size(p.Value);

		if (!val.SizeMode.empty())
		{
			std::string Format = ElementName + "->" + Format::FormatString(i.SetSizeFormat, {
				Format::FormatArg("val", Size::SizeModeToKUISizeMode(val.SizeMode))
				});
			Value += "\t" + Format + ";\n";
		}
	}

	return Value;
}

std::string UIElement::MakeCode(std::string Parent, UIElement* Root, size_t& Depth, ParseResult& MarkupElements)
{
	bool HasName = !ElementName.Empty();
	std::stringstream OutStream;
	std::string ElemName = ElementName.Empty() ? ("e_" + std::to_string(Depth)) : ElementName;
	if (!Parent.empty())
	{
		OutStream << "\t";
		if (ElementName.Empty())
		{
			OutStream << "auto* ";
		}
		std::string ClassName = TypeName;
		if (Type == ElementType::Default)
		{
			ClassName = "kui::" + ClassName;
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
				i.Name.Text.clear();
				Found = true;
				break;
			}
		}
		if (!Found && Prop.AlwaysSet)
		{
			OutStream << WriteElementProperty(this, Root, ElemName, Property{
				.Name = stringParse::StringToken(Prop.Name, 0, 0),
				.Value = Prop.Default
				}, Prop, MarkupElements);
		}
	}

	// Check if this element is any user defined type.
	for (auto& elem : MarkupElements.Elements)
	{
		if (elem.Root.TypeName != TypeName)
		{
			continue;
		}

		// If it is a user defined type, store which header declares it.
		this->Header = elem.Root.Header;
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

			OutStream << "\t" << ElemName << "->Set" << prop.Name.Text << "(" << stringParse::ToCppCode(prop.Value) << ");" << std::endl;
			prop.Name.Text.clear();
		}
	}

	for (auto& i : PropertyCopy)
	{
		if (i.Name.Text.empty())
		{
			continue;
		}
		parseError::Error("Unknown property: " + i.Name.Text, i.Name);
	}

	if (!Parent.empty())
	{
		OutStream << "\t" << Parent << "->AddChild(" << ElemName << ");" << std::endl;
	}

	// Name was set because a variable setter uses this element.
	if (!HasName && !ElementName.Empty())
	{
		// Assign the local variable of the generated element to the global one so it can be used in the setters.
		OutStream << "\t" << ElementName.Text << " = " << ElemName << ";" << std::endl;
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

bool kui::MarkupStructure::UIElement::IsDefaultElement(const std::string& Name)
{
	return GetTypeFromString(Name) != PropElementType::Unknown;
}

std::string kui::MarkupStructure::UIElement::WriteVariableSetter(std::pair<std::string, Variable> Var)
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
			Deps.insert("kui/UI/" + i.TypeName.Text + ".h");
		}
		else
		{
			Deps.insert(i.Header + ".hpp");
		}
		auto ElemDeps = i.GetElementDependencies();

		for (auto& elem : ElemDeps)
		{
			Deps.insert(elem);
		}
	}
	Deps.insert("kui/UI/UIBox.h");
	Deps.insert("kui/Markup/MarkupBox.h");
	Deps.insert("kui/Markup/Markup.h");
	return Deps;
}

std::set<std::string> kui::MarkupStructure::UIElement::GetNamedElements() const
{
	std::set<std::string> Deps;
	for (auto& i : Children)
	{
		if (!i.ElementName.Empty())
		{
			std::string Out = i.TypeName.Text + "* " + i.ElementName.Text;
			if (IsDefaultElement(i.TypeName))
			{
				Out = "kui::" + Out;
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

