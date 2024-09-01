
static std::vector<PropertyElement> Properties
{
#pragma region UITextField
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "font",
		.SetFormat = {"SetFont(KlemmUI::MarkupLanguageManager::GetActive()->GetFont({val}))"},
		.VarType = UIElement::Variable::VariableType::String,
		.AlwaysSet = true,
		.Default = "\"\""
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "textColor",
		.SetFormat = {"SetTextColor({val})"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "color",
		.SetFormat = {"SetColor({val})"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "textSize",
		.SetFormat = {"SetTextSize({val})"},
		.SetSizeFormat = "SetTextSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "hintText",
		.SetFormat = {"SetHintText({val})"},
		.VarType = UIElement::Variable::VariableType::String,
	},
#pragma endregion

#pragma region UIScrollBox
	PropertyElement{
		.Type = PropElementType::UIScrollBox,
		.Name = "displayScrollBar",
		.SetFormat = {"SetDisplayScrollBar({val}))"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
#pragma endregion

#pragma region UIText
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
		.VarType = UIElement::Variable::VariableType::SizeNumber,
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
		.Type = PropElementType::UIText,
		.Name = "wrap",
		.SetFormat = {"WrapDistance = {val}", "Wrap = true"},
		.SetSizeFormat = "WrapSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
#pragma endregion

#pragma region UIButton
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "color",
		.SetFormat = {"SetColor(Vector3f({val}))", "SetHoveredColor(Vector3f({val}) * 0.75f)", "SetPressedColor(Vector3f({val}) * 0.5f)"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "normalColor",
		.SetFormat = {"SetNormalColor(Vector3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "hoveredColor",
		.SetFormat = {"SetHoveredColor(Vector3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "pressedColor",
		.SetFormat = {"SetPressedColor(Vector3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
#pragma endregion

#pragma region UIBackground
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
		.Name = "corners",
		.SetFormat = {"CornerRadius = {val}"},
		.SetSizeFormat = "CornerSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "border",
		.SetFormat = {"BorderRadius = {val}"},
		.SetSizeFormat = "BorderSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "borderColor",
		.SetFormat = {"BorderColor = Vector3f({val})"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topLeftCorner",
		.SetFormat = {"SetCornerVisible(1, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topRightCorner",
		.SetFormat = {"SetCornerVisible(3, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomLeftCorner",
		.SetFormat = {"SetCornerVisible(0, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomRightCorner",
		.SetFormat = {"SetCornerVisible(2, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topBorder",
		.SetFormat = {"SetBorderVisible(2, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomBorder",
		.SetFormat = {"SetCornerVisible(3, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "leftBorder",
		.SetFormat = {"SetCornerVisible(1, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "rightBorder",
		.SetFormat = {"SetCornerVisible(0, {val})"},
		.VarType = UIElement::Variable::VariableType::Bool,
	},
#pragma endregion

#pragma region UIBox
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "minSize",
		.SetFormat = {"SetMinSize(Vector2f({val}))"},
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "maxSize",
		.SetFormat = {"SetMaxSize(Vector2f({val}))"},
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
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "upPadding",
		.SetFormat = {"SetUpPadding((float){val})"},
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "downPadding",
		.SetFormat = {"SetDownPadding((float){val})"},
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "leftPadding",
		.SetFormat = {"SetLeftPadding((float){val})"},
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "rightPadding",
		.SetFormat = {"SetRightPadding((float){val})"},
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
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
#pragma endregion
};
