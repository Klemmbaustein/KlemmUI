#include <Markup/MarkupVerify.h>
#include <Markup/ParseError.h>
#include <iostream>
using namespace kui::stringParse;
using namespace kui;
using namespace kui::MarkupStructure;

void markupVerify::Verify(MarkupStructure::ParseResult& Structure)
{
	for (auto& i : Structure.Elements)
	{
		parseError::SetCode(Structure.FileLines[i.File], i.File);
		VerifyElement(i.Root, i, Structure);
	}
}

void markupVerify::VerifyElement(UIElement& Element, MarkupElement& Root, MarkupStructure::ParseResult& Structure)
{
	PropElementType CurrentType = GetTypeFromString(Element.TypeName.Text);

	MarkupElement* CustomType = nullptr;
	if (GetTypeFromString(Element.TypeName) == PropElementType::Unknown && &Element != &Root.Root)
	{
		for (auto& other : Structure.Elements)
		{
			if (other.FromToken == Element.TypeName)
			{
				CustomType = &other;
			}
		}

		if (!CustomType)
		{
			parseError::Error("Unknown element type: '" + Element.TypeName.Text + "'", Element.TypeName);
		}
	}


	for (const Property& prop : Element.ElementProperties)
	{
		if (prop.Value.Empty())
		{
			parseError::Error("Expected a value after '='", prop.Value);
		}

		const PropertyElement* FoundValue = nullptr;
		for (const PropertyElement& propValue : MarkupStructure::Properties)
		{
			if (prop.Name == propValue.Name && IsSubclassOf(CurrentType, propValue.Type))
			{
				FoundValue = &propValue;
				break;
			}
		}

		if (CustomType && CustomType->Root.Variables.contains(prop.Name))
		{
			continue;
		}
		if (!FoundValue)
		{
			parseError::Error("Unknown property: '" + prop.Name.Text + "'", prop.Name);
			continue;
		}

		std::string Value = prop.Value.Text;

		Constant* FoundConstant = Structure.GetConstant(Value);
		if (FoundConstant)
		{
			Value = FoundConstant->Value;
		}

		Global* FoundGlobal = Structure.GetGlobal(Value);
		if (FoundGlobal)
		{
			if (FoundGlobal->Type == VariableType::None)
			{
				FoundGlobal->Type = FoundValue->VarType;
			}
			else if (FoundGlobal->Type != FoundValue->VarType)
			{
				parseError::Error("Global '" + Value + "' does not have the correct type for the value of '" + prop.Name.Text + "'", prop.Value);
			}
			continue;
		}

		if (Root.Root.Variables.contains(Value))
		{
			auto& Variable = Root.Root.Variables[Value];

			if (Variable.Type == VariableType::None)
			{
				Variable.Type = FoundValue->VarType;
			}
			else if (Variable.Type != FoundValue->VarType)
			{
				parseError::Error("Variable '" + Value + "' does not have the correct type for the value of '" + prop.Name.Text + "'", prop.Value);
			}
			continue;
		}

		switch (FoundValue->VarType)
		{
		case VariableType::String:
			if (!IsStringToken(Value))
				parseError::Error("Expected a string for value '" + prop.Name.Text + "'", prop.Value);
			break;

		case VariableType::Size:
			if (!IsSizeValue(Value))
				parseError::Error("Expected a size for value '" + prop.Value.Text + "'", prop.Value);
			break;

		case VariableType::SizeNumber:
			if (!Is1DSizeValue(Value))
				parseError::Error("Expected a size for value '" + prop.Name.Text + "'", prop.Value);
			break;

		case VariableType::Align:
			if (GetAlign(Value).empty())
				parseError::Error("Expected a valid align value for '" + prop.Name.Text + "'", prop.Value);
			break;

		case VariableType::Bool:
			if (Value != "true" && Value != "false")
				parseError::Error("Expected a valid boolean value for '" + prop.Name.Text + "'", prop.Value);
			break;

		case VariableType::Vector3:
		case VariableType::Vec2:
			if (!IsVectorToken(Value) && !IsNumber(Value))
			{
				parseError::Error("Expected a valid vector value for '" + prop.Name.Text + "'", prop.Value);
			}
			break;
		case VariableType::Orientation:
			if (Value != "horizontal" && Value != "vertical")
				parseError::Error("Expected a valid orientation for '" + prop.Name.Text + "'", prop.Value);
			break;

		case VariableType::SizeMode:
		default:
			break;
		}
	}

	for (auto& elem : Element.Children)
	{
		VerifyElement(elem, Root, Structure);
	}
}
