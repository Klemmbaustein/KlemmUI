#include <Markup/WriteCode.h>
#include <Markup/Format.h>
#include <sstream>
#include <iostream>

using namespace kui::stringParse;
using namespace kui::markup;

struct ConvertInfo
{
	kui::stringParse::StringToken Value;
	Constant* ValueConstant = nullptr;
	Global* ValueGlobal = nullptr;
	UIElement::Variable* Variable = nullptr;
	bool IsTranslated = false;
};

static ConvertInfo ConvertValue(UIElement* Target, UIElement* Root, StringToken Value, VariableType Type,
	ParseResult& MarkupElements)
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
		}
		if (Out.ValueGlobal)
		{
			std::string GetValue = "GetGlobal(\"" + Out.ValueGlobal->Name.Text +
				"\", kui::AnyContainer(" + kui::stringParse::ToCppCode(Out.ValueGlobal->Value) + "))";

			if (Type == VariableType::Vector3)
			{
				GetValue = GetValue + ".AsVec3()";
			}
			if (Type == VariableType::Vec2)
			{
				GetValue = GetValue + ".AsVec2()";
			}
			if (Type == VariableType::SizeNumber)
			{
				GetValue = GetValue + ".AsSize()";
			}
			if (Type == VariableType::Size)
			{
				GetValue = GetValue + ".AsSizeVec()";
			}
			Value.Text = GetValue;
		}
	}
	else
	{

		switch (Type)
		{
		case VariableType::String:
		case VariableType::Size:
		case VariableType::SizeNumber:
			break;

		case VariableType::Align:
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

kui::markup::ElementWriter::ElementWriter(MarkupElement* ToWrite, ParseResult* Parsed)
{
	this->ToWrite = ToWrite;
	this->Parsed = Parsed;
}

void kui::markup::ElementWriter::Write(std::ofstream& Stream)
{
	Stream << "\nclass " << ToWrite->Root.TypeName.Text
		+ " : public kui::UIBox, public kui::markup::MarkupBox\n{\n";

	Stream << "public:\n";

	std::stringstream ConstructorStream;

	WriteConstructor(ConstructorStream);

	Stream << "\n\t// Child elements\n";

	for (auto& i : Elements)
	{
		if (!IsNameUnnamed(i.first))
		{
			Stream << "\t" << ToCppTypeName(i.second->TypeName) << "* " << i.second->ElementName.Text << ";\n";
		}
	}

	Stream << "\nprivate:\n";
	Stream << "\t// Private child elements\n";

	for (auto& i : Elements)
	{
		if (IsNameUnnamed(i.first))
		{
			Stream << "\t" << ToCppTypeName(i.second->TypeName) << "* " << i.second->ElementName.Text << ";\n";
		}
	}
	Stream << "\n\t// Variables\n";

	for (auto& i : ToWrite->Root.Variables)
	{
		if (i.second.Type != VariableType::None)
		{
			Stream << "\t" << UIElement::Variable::Descriptions[i.second.Type].CppName
				<< " " << i.second.Token.Text;

			if (!i.second.Value.empty())
			{
				Constant* ValueConstant = Parsed->GetConstant(i.second.Value);

				if (ValueConstant)
				{
					i.second.Value = ValueConstant->Value;
				}

				std::string Value = stringParse::ToCppCode(i.second.Value);

				Stream << " = " << Value;
			}
			else
			{
				Stream << "{}";
			}
			Stream << ";\n";
		}
		else
		{
			Stream << "\t// Unused: " << i.second.Token.Text << "\n";
		}
	}

	Stream << "\npublic:\n";

	Stream << ConstructorStream.str();

	Stream << "\n\t// Variable Setters\n";

	for (auto& i : ToWrite->Root.Variables)
	{
		WriteVariableSetter(Stream, i.second);
	}

	WriteOnTranslationChanged(Stream);
	WriteOnGlobalChanged(Stream);

	Stream << "};\n";
}

void kui::markup::ElementWriter::WriteOnGlobalChanged(std::ostream& Stream)
{
	if (ToWrite->Root.GlobalProperties.empty())
	{
		return;
	}

	Stream << "\tvirtual void OnGlobalChanged() override\n\t{\n";
	for (auto& i : ToWrite->Root.GlobalProperties)
	{
		Global* ValueGlobal = Parsed->GetGlobal(i.Value);

		Stream << Format::FormatString(i.Name, { Format::FormatArg("val", i.Value.Text) });
	}
	Stream << "\t}\n";
}

void kui::markup::ElementWriter::WriteOnTranslationChanged(std::ostream& Stream)
{
	if (ToWrite->Root.TranslatedProperties.empty())
	{
		return;
	}

	Stream << "\tvirtual void OnTranslationChanged() override\n\t{\n";
	for (auto& i : ToWrite->Root.TranslatedProperties)
	{
		Stream << "\t\t" << Format::FormatString(i.Name, { Format::FormatArg("val",
			kui::stringParse::ToCppCode(i.Value)) }) << ";\n";
	}
	Stream << "\t}\n";
}

bool kui::markup::ElementWriter::IsNameUnnamed(std::string Name)
{
	return Name.substr(0, 8) == "Unnamed_";
}

void kui::markup::ElementWriter::GetDependencies(std::set<std::string>& OutDependencies)
{
	GetDependenciesForElement(OutDependencies, &ToWrite->Root);
}

void kui::markup::ElementWriter::GetDependenciesForElement(std::set<std::string>& OutDependencies,
	UIElement* Element)
{
	OutDependencies.insert(Element->TypeName);

	for (auto& i : Element->Children)
	{
		GetDependenciesForElement(OutDependencies, &i);
	}
}

void kui::markup::ElementWriter::GetElementDependencies(std::set<std::string>& OutDependencies)
{
	GetDependenciesForElement(OutDependencies, &ToWrite->Root);
}

void kui::markup::ElementWriter::GetElementDependenciesForElement(std::set<std::string>& OutDependencies,
	UIElement* Element)
{
	if (!UIElement::IsDefaultElement(Element->TypeName))
	{
		OutDependencies.insert(Element->TypeName);
	}

	for (auto& i : Element->Children)
	{
		GetDependenciesForElement(OutDependencies, &i);
	}
}

std::string kui::markup::ElementWriter::RepeatedTabs(size_t Length)
{
	std::string s;
	s.resize(Length, '\t');

	return s;
}

void kui::markup::ElementWriter::WriteConstructor(std::ostream& Stream)
{
	Stream << "\t" << ToWrite->Root.TypeName.Text << "() : kui::UIBox(true)" << "\n\t{\n";

	WriteElementConstructor(Stream, ToWrite->Root, 2, true, "");

	Stream << "\t}\n";
}

static std::map<PropElementType, std::string> DefaultConstructors =
{
	{PropElementType::UIBox, "true"},
	{PropElementType::UIText, "kui::UISize(1), 1, \"\", nullptr"},
	{PropElementType::UIButton, "true, 0, 1, nullptr"},
	{PropElementType::UIBackground, "true, 0, 1"},
	{PropElementType::UITextField, "0, 1, nullptr, nullptr"},
	{PropElementType::UIBlurBackground, "true, 0, 1"},
	{PropElementType::UISpinner, "0, 1"},
	{PropElementType::UIScrollBox, "true, 0, true"},
};

std::string kui::markup::ElementWriter::ToCppTypeName(stringParse::StringToken Name)
{
	if (UIElement::IsDefaultElement(Name.Text))
	{
		return "kui::" + Name.Text;
	}
	return Name.Text;
}

void kui::markup::ElementWriter::WriteVariableSetter(std::ostream& Stream, UIElement::Variable& Var)
{
	Stream << "\tvoid Set" << Var.Token.Text
		<< "(" << UIElement::Variable::Descriptions[Var.Type].CppName << " NewValue)\n\t{" << std::endl;
	Stream << "\t\t" << Var.Token.Text << " = NewValue;\n";
	for (auto& i : Var.References)
	{
		Stream << "\t\t" << Format::FormatString(i, { Format::FormatArg("val", "NewValue") }) << ";" << std::endl;
	}
	Stream << "\t}\n";
}

void kui::markup::ElementWriter::WriteElementConstructor(std::ostream& Stream,
	UIElement& Target, size_t Depth, bool IsThis, std::string Parent)
{
	if (Target.ElementName.Empty() && !IsThis)
	{
		Target.ElementName = StringToken("Unnamed_" + std::to_string(this->UnnamedCounter++), 0, 0);
	}
	else if (Target.ElementName.Empty())
	{
		Target.ElementName = StringToken("this", 0, 0);
	}

	std::string ElementValue = Target.ElementName.Text;

	Stream << RepeatedTabs(Depth) << "// Initialize " << Target.TypeName.Text << "* " << ElementValue << "\n";

	if (!IsThis)
	{
		Elements[ElementValue] = &Target;
		Stream << RepeatedTabs(Depth) << ElementValue << " = new "
			<< ToCppTypeName(Target.TypeName) << "("
			<< DefaultConstructors[GetTypeFromString(Target.TypeName)] << ");\n";

		Stream << RepeatedTabs(Depth) << Parent << "->AddChild(" << ElementValue << ");\n";
	}

	WriteProperties(Stream, Target, Depth);

	for (auto& i : Target.Children)
	{
		WriteElementConstructor(Stream, i, Depth, false, Target.ElementName.Text);
	}
}

void kui::markup::ElementWriter::WriteProperties(std::ostream& Stream, UIElement& Target, size_t Depth)
{
	// Built in default propertise
	for (auto& Prop : Properties)
	{
		if (!IsSubclassOf(GetTypeFromString(Target.TypeName), Prop.Type))
		{
			continue;
		}
		bool Found = false;
		for (auto& i : Target.ElementProperties)
		{
			if (i.Name == Prop.Name)
			{
				WriteProperty(i.Value, Prop, Stream, Target, Depth);
				i.Name.Text.clear();
				Found = true;
				break;
			}
		}
		if (!Found && Prop.AlwaysSet)
		{
			WriteProperty(stringParse::StringToken(Prop.Default, 0, 0), Prop, Stream, Target, Depth);
		}
	}

	// User defined properties via variables
	if (!UIElement::IsDefaultElement(Target.TypeName))
	{
		auto ParentType = Parsed->GetElement(Target.TypeName);

		for (auto& v : ParentType->Root.Variables)
		{
			for (auto& p : Target.ElementProperties)
			{
				if (v.first == p.Name)
				{
					WriteSetVariable(p.Value, v.second, Stream, Target, Depth);
				}
			}
		}
	}
}

void kui::markup::ElementWriter::WriteProperty(stringParse::StringToken Value, PropertyElement& FoundProperty,
	std::ostream& Stream, UIElement& Target, size_t Depth)
{
	std::string TargetName = Target.ElementName.Text;

	auto Result = ConvertValue(&Target, &ToWrite->Root, Value, FoundProperty.VarType, *Parsed);

	if (FoundProperty.CreateCodeFunction)
	{
		Stream << RepeatedTabs(Depth) << TargetName << "->" << FoundProperty.CreateCodeFunction(Value.Text) << ";\n";
	}

	if (Result.IsTranslated)
	{
		ToWrite->Root.TranslatedProperties.push_back(Property(
			StringToken(Target.ElementName.Text + "->" + FoundProperty.SetFormat[0], 0, 0), Result.Value));
	}
	if (Result.ValueGlobal)
	{
		std::string SetFormat;

		for (auto& FormatElement : FoundProperty.SetFormat)
		{
			SetFormat += "\t\t" + Target.ElementName.Text + "->" + FormatElement + ";\n";
		}

		ToWrite->Root.GlobalProperties.push_back(Property(StringToken(SetFormat, 0, 0), Result.Value));
	}
	for (auto& FormatString : FoundProperty.SetFormat)
	{
		std::string SetValue;

		if (Result.ValueGlobal)
		{
			SetValue = Result.Value.Text;
		}
		else if (Result.Variable)
		{
			SetValue = Result.Value.Text;
		}
		else if (FoundProperty.VarType == VariableType::Size
			|| FoundProperty.VarType == VariableType::SizeNumber)
		{
			auto val = Size(Value);
			SetValue = val.ToCppCode(FoundProperty.VarType == VariableType::Size);
		}
		else
		{
			SetValue = kui::stringParse::ToCppCode(Result.Value);
		}

		std::string Format = Target.ElementName.Text + "->"
			+ Format::FormatString(FormatString, { Format::FormatArg("val", SetValue) });

		Stream << RepeatedTabs(Depth) + Format + ";\n";
		if (Result.Variable)
		{
			Result.Variable->References.push_back(Target.ElementName.Text + "->" + FormatString);
		}
	}
}

void kui::markup::ElementWriter::WriteSetVariable(stringParse::StringToken Value, UIElement::Variable& FoundVariable,
	std::ostream& Stream, UIElement& Target, size_t Depth)
{
	if (FoundVariable.Type == VariableType::None)
	{
		return;
	}

	std::string TargetName = Target.ElementName.Text;

	auto Result = ConvertValue(&Target, &ToWrite->Root, Value, FoundVariable.Type, *Parsed);

	std::string SetFormat = TargetName + "->Set" + FoundVariable.Token.Text + "({val})";

	if (Result.IsTranslated)
	{
		ToWrite->Root.TranslatedProperties.push_back(Property(
			StringToken(SetFormat, 0, 0), Result.Value));
	}
	if (Result.ValueGlobal)
	{
		ToWrite->Root.GlobalProperties.push_back(Property(StringToken(SetFormat, 0, 0), Result.Value));
	}
	std::string SetValue;

	if (Result.ValueGlobal)
	{
		SetValue = Result.Value.Text;
	}
	else if (Result.Variable)
	{
		SetValue = Result.Value.Text;
	}
	else if (FoundVariable.Type == VariableType::Size
		|| FoundVariable.Type == VariableType::SizeNumber)
	{
		auto val = Size(Value);
		SetValue = val.ToCppCode(FoundVariable.Type == VariableType::Size);
	}
	else
	{
		SetValue = kui::stringParse::ToCppCode(Result.Value);
	}

	std::string Format = Format::FormatString(SetFormat, { Format::FormatArg("val", SetValue) });

	Stream << RepeatedTabs(Depth) + Format + ";\n";
	if (Result.Variable)
	{
		Result.Variable->References.push_back(SetFormat);
	}
}