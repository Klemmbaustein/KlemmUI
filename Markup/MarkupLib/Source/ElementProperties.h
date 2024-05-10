
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
		.Type = PropElementType::UIButton,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))", "SetHoveredColor(Vector3f({val}) * 0.75f)", "SetPressedColor(Vector3f({val}) * 0.5f)"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},


	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "opacity",
		.SetFormat = {"SetOpacity({val})"},
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "image",
		.SetFormat = {"SetUseTexture(true, {val})"},
		.VarType = UIElement::Variable::VariableType::String,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "borderType",
		.SetFormat = {"BoxBorder = {val}"},
		.VarType = UIElement::Variable::VariableType::BorderType,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "borderSize",
		.SetFormat = {"BorderRadius = {val}"},
		.SetSizeFormat = "BorderSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "borderColor",
		.SetFormat = {"BorderColor = Vector3f({val})"},
		.VarType = UIElement::Variable::VariableType::Vector3,
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
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "maxSize",
		.SetFormat = {"SetMaxSize({val})"},
		.SetSizeFormat = "SetSizeMode({val})",
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
		.SetFormat = {"SetVerticalAlign({val})", "SetHorizontalAlign({val})"},
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "horizontalAlign",
		.SetFormat = {"SetHorizontalAlign({val})"},
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "verticalAlign",
		.SetFormat = {"SetVerticalAlign({val})"},
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
			if (Val == "horizontal")
			{
				return "SetHorizontal(true)";
			}
			if (Val == "vertical")
			{
				return "SetHorizontal(false)";
			}
			KlemmUI::ParseError::ErrorNoLine("Invalid orientation: '" + Val + "'");
			return "";
		},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
};
