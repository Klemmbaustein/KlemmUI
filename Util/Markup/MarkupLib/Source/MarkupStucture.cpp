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


std::map<VariableType, UIElement::Variable::VariableTypeDescription> UIElement::Variable::Descriptions =
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
	VariableTypeDescription("SizeVector", "kui::Vec2f"),
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
	VariableTypeDescription("Vector3", "kui::Vec3f"),
},
{
	VariableType::Vec2,
	VariableTypeDescription("Vec2", "kui::Vec2f"),
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
	{PropElementType::UIBlurBackground, "true, 0, 1"},
	{PropElementType::UISpinner, "0, 1"},
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
	Out << "\t}\n";

	Out << "\tvirtual void OnGlobalChanged() override\n\t{\n";
	for (auto& i : Root.GlobalProperties)
	{
		Global* ValueGlobal = MarkupElements.GetGlobal(i.Value);
		
		Out << "\t\t" << Format::FormatString(i.Name, { Format::FormatArg("val", i.Value.Text) }) << "\n";
	}
	Out << "\t}\n";


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
	if (TypeName == "UIBlurBackground")
	{
		return PropElementType::UIBlurBackground;
	}
	if (TypeName == "UISpinner")
	{
		return PropElementType::UISpinner;
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
	if (Type == PropElementType::UIBlurBackground)
	{
		return "UIBlurBackground";
	}
	if (Type == PropElementType::UISpinner)
	{
		return "UISpinner";
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

Global* ParseResult::GetGlobal(std::string Name)
{
	for (Global& i : Globals)
	{
		if (i.Name == Name)
		{
			return &i;
		}
	}
	return nullptr;
}

struct ConvertInfo
{
	kui::stringParse::StringToken Value;
	Constant* ValueConstant = nullptr;
	Global* ValueGlobal = nullptr;
	UIElement::Variable* Variable = nullptr;
	bool IsTranslated = false;
};

static ConvertInfo ConvertValue(UIElement* Target, UIElement* Root, kui::stringParse::StringToken Value, VariableType Type, std::string ValueName, ParseResult& MarkupElements)
{
	ConvertInfo Out;
	using namespace kui::stringParse;
	auto MapVariable = Root->Variables.find(Value);

	Out.Variable = MapVariable != Root->Variables.end() ? &MapVariable->second : nullptr;
	Out.ValueConstant = MarkupElements.GetConstant(Value);

	if (Out.ValueConstant)
	{
		Value = StringToken(Out.ValueConstant->Value, 0, 0);
	}

	Out.ValueGlobal = MarkupElements.GetGlobal(Value);

	Out.IsTranslated = IsTranslatedString(Value) && Type == VariableType::String;

	if (Out.Variable || Out.IsTranslated || Out.ValueGlobal)
	{
		if (Out.Variable && Out.Variable->Type != Type)
		{
			if (Out.Variable->Type == VariableType::None)
			{
				Out.Variable->Type = Type;
			}
			else
			{
				kui::parseError::ErrorNoLine("Variable '" + Out.Variable->Token.Text + "' does not have the correct type for the value of '" + ValueName + "'");
			}
		}
		if (Target->ElementName.Empty() && Target != Root)
		{
			Target->ElementName = StringToken("unnamed_" + std::to_string(UnnamedCounter++), 0, 0);
		}
		if (Out.ValueGlobal)
		{
			std::string GetValue = "GetGlobal(\"" + Out.ValueGlobal->Name.Text + "\", kui::AnyContainer(" + kui::stringParse::ToCppCode(Out.ValueGlobal->Value) + "))";
			if (Type == VariableType::Vector3)
			{
				GetValue = GetValue + ".AsVec3()";
			}
			if (Type == VariableType::Vec2)
			{
				GetValue = GetValue + ".AsVec2()";
			}
			Value.Text = GetValue;
		}
	}
	else
	{

		switch (Type)
		{
		case VariableType::String:
			if (!IsStringToken(Value))
				kui::parseError::ErrorNoLine("Expected a string for value '" + ValueName + "'");
			break;

		case VariableType::Size:
			if (!IsSizeValue(Value))
				kui::parseError::ErrorNoLine("Expected a size for value '" + ValueName + "'");
			break;

		case VariableType::SizeNumber:
			if (!Is1DSizeValue(Value))
				kui::parseError::ErrorNoLine("Expected a size for value '" + ValueName + "'");
			break;

		case VariableType::Align:
			if (GetAlign(Value).empty())
				kui::parseError::ErrorNoLine("Expected a valid align value for '" + ValueName + "'");
			Value = StringToken(GetAlign(Value), 0, 0);
			break;

		case VariableType::SizeMode:
			Value = StringToken(Size::SizeModeToKUISizeMode(Value), 0, 0);
			break;

		case VariableType::Vector3:
		case VariableType::Vec2:
			if (!IsVectorToken(Value) || IsNumber(Value))
			{
				Value = StringToken("(" + Value.Text + ")", 0, 0);
			}
			break;
		default:
			break;
		}
	}
	Out.Value = Value;

	return Out;
}

static std::string WriteElementProperty(UIElement* Target, UIElement* Root, std::string ElementName, Property p, const PropertyElement& i, ParseResult& MarkupElements)
{
	using namespace kui::stringParse;

	ConvertInfo Result = ConvertValue(Target, Root, p.Value, i.VarType, i.Name, MarkupElements);
	if (Result.IsTranslated)
	{
		Root->TranslatedProperties.push_back(Property(StringToken(Target->ElementName.Text + "->" + i.SetFormat[0], 0, 0), Result.Value));
	}
	if (Result.ValueGlobal)
	{
		std::string SetFormat;

		for (auto& FormatElement : i.SetFormat)
		{
			SetFormat += Target->ElementName.Text + "->" + FormatElement + ";\n\t\t";
		}

		Root->GlobalProperties.push_back(Property(StringToken(SetFormat, 0, 0), Result.Value));
	}

	if (i.CreateCodeFunction)
	{
		return "\t\t" + ElementName + "->" + i.CreateCodeFunction(Result.Value) + ";\n";
	}

	std::string Value;

	for (auto& FormatString : i.SetFormat)
	{
		std::string SetValue;

		if (Result.ValueGlobal)
		{
			SetValue = Result.Value.Text;
		}
		else
		{
			SetValue = kui::stringParse::ToCppCode(Result.Value);
		}

		std::string Format = "->" + Format::FormatString(FormatString, { Format::FormatArg("val", SetValue) });

		Value += "\t\t" + ElementName + Format + ";\n";
		if (Result.Variable)
		{
			Result.Variable->References.push_back(Target->ElementName.Text + Format);
		}
	}

	if (i.VarType == VariableType::Size || i.VarType == VariableType::SizeNumber && !Result.ValueGlobal)
	{
		auto val = Size(p.Value);

		if (!val.SizeMode.empty())
		{
			std::string Format = ElementName + "->" + Format::FormatString(i.SetSizeFormat, {
				Format::FormatArg("val", Size::SizeModeToKUISizeMode(val.SizeMode))
				});
			Value += "\t\t" + Format + ";\n";
		}
	}

	return Value;
}

std::string UIElement::MakeCode(std::string Parent, UIElement* Root, size_t& Depth, ParseResult& MarkupElements)
{
	using namespace kui::stringParse;
	
	bool HasName = !ElementName.Empty();
	std::stringstream OutStream;
	std::string ElemName = ElementName.Empty() ? ("e_" + std::to_string(Depth)) : ElementName;
	if (!Parent.empty())
	{
		OutStream << "\t\t";
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
				.Value = stringParse::StringToken(Prop.Default, 0, 0)
				}, Prop, MarkupElements);
		}
	}

	// Check if this element is any user defined type.
	// Variables can be set in the same way as predefined element properties.
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
			auto Variable = elem.Root.Variables.find(prop.Name);
			if (Variable == elem.Root.Variables.end())
			{
				continue;
			}

			ConvertInfo Result = ConvertValue(this, Root, prop.Value, Variable->second.Type, Variable->second.Token, MarkupElements);

			std::string SetFormat = ElemName + "->Set" + prop.Name.Text + "({val})";

			if (Result.IsTranslated)
			{
				Root->TranslatedProperties.push_back(Property(StringToken(SetFormat, 0, 0), Result.Value));
			}
			if (Result.ValueGlobal)
			{
				std::string ThisElementName = this->ElementName.Text;
				if (ThisElementName.empty())
				{
					ThisElementName = ElemName;
				}
				std::string SetFormat = ThisElementName + "->Set" + prop.Name.Text + "({val})";
				Root->GlobalProperties.push_back(Property(StringToken(SetFormat + ";", 0, 0), Result.Value));
			}

			OutStream << "\t\t" << Format::FormatString(SetFormat, {Format::FormatArg("val", ToCppCode(Result.Value))}) << ";" << std::endl;
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
		OutStream << "\t\t" << Parent << "->AddChild(" << ElemName << ");" << std::endl;
	}

	// Name was set because a variable setter uses this element.
	if (!HasName && !ElementName.Empty())
	{
		// Assign the local variable of the generated element to the global one so it can be used in the setters.
		OutStream << "\t\t" << ElementName.Text << " = " << ElemName << ";" << std::endl;
	}

	if (!Children.empty())
	{
		OutStream << "\t\t{\n";
		for (auto& i : Children)
		{
			OutStream << i.MakeCode(ElemName, Root, ++Depth, MarkupElements);
		}
		OutStream << "\t\t}\n";
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
		else if (!i.Header.empty())
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

