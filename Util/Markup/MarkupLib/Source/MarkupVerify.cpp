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

void markupVerify::VerifyElement(UIElement& Element, const MarkupElement& Root, MarkupStructure::ParseResult& Structure)
{
	for (const std::pair<std::string, UIElement::Variable>& var : Element.Variables)
	{
	}
	PropElementType CurrentType = GetTypeFromString(Element.TypeName.Text);

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

		if (!FoundValue)
		{
			parseError::Error("Unknown property: '" + prop.Name.Text + "'", prop.Name);
			continue;
		}

		std::string Value = prop.Value.Text;

		for (auto& i : Structure.Constants)
		{
			if (i.Name == Value)
			{
				Value = i.Value;
			}
		}
		
		if (Root.Root.Variables.contains(Value))
		{
			auto& Variable = Element.Variables[Value];

			if (Variable.Type == UIElement::Variable::VariableType::None)
			{
				Variable.Type = FoundValue->VarType;
			}
			else
			{
				parseError::Error("Variable '" + Value + "' does not have the correct type for the value of '" + prop.Name.Text + "'", prop.Value);
			}
			continue;
		}

		switch (FoundValue->VarType)
		{
		case UIElement::Variable::VariableType::String:
			if (!IsStringToken(Value))
				parseError::Error("Expected a string for value '" + prop.Name.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::Size:
			if (!IsSizeValue(Value))
				parseError::Error("Expected a size for value '" + prop.Value.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::SizeNumber:
			if (!Is1DSizeValue(Value))
				parseError::Error("Expected a size for value '" + prop.Name.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::Align:
			if (GetAlign(Value).empty())
				parseError::Error("Expected a valid align value for '" + prop.Name.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::Bool:
			if (Value != "true" && Value != "false")
				parseError::Error("Expected a valid boolean value for '" + prop.Name.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::Vector3:
		case UIElement::Variable::VariableType::Vec2:
			if (!IsVectorToken(Value) && !IsNumber(Value))
			{
				parseError::Error("Expected a valid vector value for '" + prop.Name.Text + "'", prop.Value);
			}
			break;
		case UIElement::Variable::VariableType::Orientation:
			if (Value != "horizontal" && Value != "vertical")
				parseError::Error("Expected a valid orientation for '" + prop.Name.Text + "'", prop.Value);
			break;

		case UIElement::Variable::VariableType::SizeMode:
		default:
			break;
		}
	}

	for (auto& elem : Element.Children)
	{
		if (GetTypeFromString(elem.TypeName) == PropElementType::Unknown)
		{
			bool Found = false;
			for (auto& other : Structure.Elements)
			{
				if (other.FromToken == elem.TypeName)
				{
					Found = true;
				}
			}

			if (!Found)
			{
				parseError::Error("Unknown element type: '" + elem.TypeName.Text + "'", elem.TypeName);
			}
		}
		VerifyElement(elem, Root, Structure);
	}
}
