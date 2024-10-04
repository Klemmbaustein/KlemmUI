
std::vector<PropertyElement> kui::MarkupStructure::Properties
{
#pragma region UISpinner
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "backgroundColor",
		.Description = "The background color of the spinner.",
		.SetFormat = {"SetBackgroundColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
#pragma endregion
#pragma region UITextField
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "font",
		.Description = "The name of the font used by the text in the text field",
		.SetFormat = {"SetFont(kui::MarkupLanguageManager::GetActive()->GetFont({val}))"},
		.VarType = UIElement::Variable::VariableType::String,
		.AlwaysSet = true,
		.Default = "\"\""
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "textColor",
		.Description = "The color value of the text in the text field.",
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
		.Description = "The size of the text in the text field.",
		.SetFormat = { "SetTextSize({val})" },
		.SetSizeFormat = "SetTextSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UITextField,
		.Name = "hintText",
		.Description = "The text displayed when there is no text entered in the text field.",
		.SetFormat = { "SetHintText({val})" },
		.VarType = UIElement::Variable::VariableType::String,
	},
#pragma endregion

#pragma region UIScrollBox
	PropertyElement{
		.Type = PropElementType::UIScrollBox,
		.Name = "displayScrollBar",
		.Description = "True if the scroll box should have a scroll bar.",
		.SetFormat = { "SetDisplayScrollBar({val}))" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
#pragma endregion

#pragma region UIText
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "text",
		.Description = "The text string.",
		.SetFormat = { "SetText({val})" },
		.VarType = UIElement::Variable::VariableType::String,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "sizeMode",
		.Description = "The size mode of `UIText.size`.",
		.SetFormat = { "SetTextSizeMode({val})" },
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "color",
		.Description = "The color of the text.",
		.SetFormat = {"SetColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "size",
		.Description = "The size of the text.",
		.SetFormat = { "SetTextSize({val})" },
		.SetSizeFormat = "SetTextSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "font",
		.Description = "The name of the font used by the text.",
		.SetFormat = {"SetFont(kui::MarkupLanguageManager::GetActive()->GetFont({val}))"},
		.VarType = UIElement::Variable::VariableType::String,
		.AlwaysSet = true,
		.Default = "\"\""
	},
	PropertyElement{
		.Type = PropElementType::UIText,
		.Name = "wrap",
		.Description = "The distance before the text should wrap around.",
		.SetFormat = { "WrapDistance = {val}", "Wrap = true" },
		.SetSizeFormat = "WrapSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
#pragma endregion

#pragma region UIButton
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "color",
		.Description = "The color of the button. This sets normalColor, hoveredColor and pressedColor to reasonable values from the given color.",
		.SetFormat = {"SetColor(kui::Vec3f({val}))", "SetHoveredColor(kui::Vec3f({val}) * 0.75f)", "SetPressedColor(kui::Vec3f({val}) * 0.5f)"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "normalColor",
		.Description = "The normal color of the button, when it isn't pressed or hovered.",
		.SetFormat = {"SetColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "hoveredColor",
		.Description = "The color of the button when it is hovered.",
		.SetFormat = {"SetHoveredColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIButton,
		.Name = "pressedColor",
		.Description = "The color of the button when it is pressed.",
		.SetFormat = {"SetPressedColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
#pragma endregion

#pragma region UIBackground
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "color",
		.Description = "The color of the background.",
		.SetFormat = {"SetColor(kui::Vec3f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "opacity",
		.Description = "The color opacity of the background. 0 is invisible, 1 is fully visible.",
		.SetFormat = { "SetOpacity({val})" },
		.VarType = UIElement::Variable::VariableType::Number,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "image",
		.Description = "The name of an image file for the background.",
		.SetFormat = { "SetUseTexture(true, {val})" },
		.VarType = UIElement::Variable::VariableType::String,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "corners",
		.Description = "The size of rounded corners for the background.",
		.SetFormat = { "CornerRadius = {val}" },
		.SetSizeFormat = "CornerSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "border",
		.Description = "The size of a border for the background. Use borderColor to set the color of the border",
		.SetFormat = { "BorderRadius = {val}" },
		.SetSizeFormat = "BorderSizeMode = {val}",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "borderColor",
		.Description = "The color of the border for the background.",
		.SetFormat = {"BorderColor = kui::Vec3f({val})"},
		.VarType = UIElement::Variable::VariableType::Vector3,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topLeftCorner",
		.Description = "Should the top left corner of the background be rounded.",
		.SetFormat = { "SetCornerVisible(1, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topRightCorner",
		.Description = "Should the top right corner of the background be rounded.",
		.SetFormat = { "SetCornerVisible(3, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomLeftCorner",
		.Description = "Should the bottom left corner of the background be rounded.",
		.SetFormat = { "SetCornerVisible(0, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomRightCorner",
		.Description = "Should the bottom right corner of the background be rounded.",
		.SetFormat = { "SetCornerVisible(2, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "topBorder",
		.Description = "Should the top border of the background use the borderColor color.",
		.SetFormat = { "SetBorderVisible(2, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "bottomBorder",
		.Description = "Should the bottom border of the background use the borderColor color.",
		.SetFormat = { "SetCornerVisible(3, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "leftBorder",
		.Description = "Should the left border of the background use the borderColor color.",
		.SetFormat = { "SetCornerVisible(1, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBackground,
		.Name = "rightBorder",
		.Description = "Should the right border of the background use the borderColor color.",
		.SetFormat = { "SetCornerVisible(0, {val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
#pragma endregion

#pragma region UIBox
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "minSize",
		.Description = "The minimum size of this element.",
		.SetFormat = {"SetMinSize(Vec2f({val}))"},
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "maxSize",
		.Description = "The maximum size of this element.",
		.SetFormat = {"SetMaxSize(Vec2f({val}))"},
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "size",
		.Description = "Sets both minSize and maxSize to the given value.",
		.SetFormat = {"SetMinSize(Vec2f({val}))", "SetMaxSize(Vec2f({val}))"},
		.SetSizeFormat = "SetSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::Size,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "position",
		.Description = "The position of the element, from -1 (bottom left corner) to 1 (top right corner).",
		.SetFormat = {"SetPosition(Vec2f({val}))"},
		.VarType = UIElement::Variable::VariableType::Vec2,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "allAlign",
		.Description = "Sets the horizontal and vertical align of this element's children.",
		.SetFormat = { "SetVerticalAlign({val})", "SetHorizontalAlign({val})" },
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "horizontalAlign",
		.Description = "Sets the horizontal align of this element's children.",
		.SetFormat = { "SetHorizontalAlign({val})" },
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "verticalAlign",
		.Description = "Sets the vertical align of this element's children.",
		.SetFormat = { "SetVerticalAlign({val})" },
		.VarType = UIElement::Variable::VariableType::Align,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "paddingSizeMode",
		.Description = "Sets the size mode of this element's padding values.",
		.SetFormat = { "SetPaddingSizeMode({val})" },
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "sizeMode",
		.Description = "Sets the size mode of this element's size.",
		.SetFormat = { "SetSizeMode({val})" },
		.VarType = UIElement::Variable::VariableType::SizeMode,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "padding",
		.Description = "The padding of this element in all directions.",
		.SetFormat = { "SetPadding((float){val})" },
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "upPadding",
		.Description = "The up padding of this element.",
		.SetFormat = { "SetUpPadding((float){val})" },
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "downPadding",
		.Description = "The down padding of this element.",
		.SetFormat = { "SetDownPadding((float){val})" },
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "leftPadding",
		.Description = "The left padding of this element.",
		.SetFormat = { "SetLeftPadding((float){val})" },
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "rightPadding",
		.Description = "The right padding of this element.",
		.SetFormat = { "SetRightPadding((float){val})" },
		.SetSizeFormat = "SetPaddingSizeMode({val})",
		.VarType = UIElement::Variable::VariableType::SizeNumber,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "hasMouseCollision",
		.Description = "True if this element has mouse collision. If it has mouse collision, it will block hovering or clicking anything below it.",
		.SetFormat = { "HasMouseCollision((bool){val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "tryFill",
		.Description = "True if this element should completely fill up the parent's size in the direction opposite of UIBox.orientation.",
		.SetFormat = { "SetTryFill((bool){val})" },
		.VarType = UIElement::Variable::VariableType::Bool,
	},
	PropertyElement{
		.Type = PropElementType::UIBox,
		.Name = "orientation",
		.Description = "The orientation of this element. `vertical` means all children are arranged vertically, `horizontal` means the children are arranged horizontally.",
		.CreateCodeFunction = [](std::string Val) -> std::string {
			if (Val == "horizontal")
			{
				return "SetHorizontal(true)";
			}
			if (Val == "vertical")
			{
				return "SetHorizontal(false)";
			}
			kui::parseError::ErrorNoLine("Invalid orientation: '" + Val + "'");
			return "";
		},
		.VarType = UIElement::Variable::VariableType::Orientation,
	},
#pragma endregion
};
