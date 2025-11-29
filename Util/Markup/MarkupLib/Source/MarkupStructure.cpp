#include <Markup/MarkupStructure.h>

using namespace kui;
using namespace kui::markup;

std::map<VariableType, UIElement::Variable::VariableTypeDescription> UIElement::Variable::Descriptions = {
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
	VariableTypeDescription("SizeVector", "kui::SizeVec"),
},
{
	VariableType::SizeNumber,
	VariableTypeDescription("SizeValue", "kui::UISize"),
},
{
	VariableType::SizeMode,
	VariableTypeDescription("SizeMode", "kui::SizeMode"),
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
};

PropElementType kui::markup::GetTypeFromString(std::string TypeName)
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

bool kui::markup::UIElement::IsDefaultElement(const std::string& Name)
{
	return GetTypeFromString(Name) != PropElementType::Unknown;
}

std::string kui::markup::GetStringFromType(PropElementType Type)
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

bool kui::markup::IsSubclassOf(PropElementType Class, PropElementType Parent)
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

Global* kui::markup::ParseResult::GetGlobal(std::string Name)
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

Constant* kui::markup::ParseResult::GetConstant(std::string Name)
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

MarkupElement* kui::markup::ParseResult::GetElement(std::string Name)
{
	for (MarkupElement& i : Elements)
	{
		if (i.Root.TypeName == Name)
		{
			return &i;
		}
	}
	return nullptr;
}