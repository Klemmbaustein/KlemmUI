// The unreadable (rustified) version of MarkupLib/Source/ElementProperties.h

std::vector<DynamicProperty> DynamicProperties
{
	DynamicProperty{ .Name = "font",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetFont(Window::GetActiveWindow()->Markup.GetFont(Value));
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "textColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetTextColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "color",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "textSize",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetTextSize(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "hintText",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetHintText(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "text",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetText(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "canEdit",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetCanEdit(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{.Name = "innerPadding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetInnerPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "allowNewLine",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UITextField*)Target)->SetAllowNewLine(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UITextField*>(Text);
	} },

	DynamicProperty{ .Name = "displayScrollBar",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIScrollBox*)Target)->SetDisplayScrollBar(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIScrollBox*>(Text);
	} },

	DynamicProperty{ .Name = "text",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIText*)Target)->SetText(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIText*>(Text);
	} },

	DynamicProperty{ .Name = "color",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIText*)Target)->SetColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIText*>(Text);
	} },

	DynamicProperty{ .Name = "size",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIText*)Target)->SetTextSize(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIText*>(Text);
	} },

	DynamicProperty{ .Name = "font",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIText*)Target)->SetFont(Window::GetActiveWindow()->Markup.GetFont(Value));
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIText*>(Text);
	} },

	DynamicProperty{ .Name = "wrap",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIText*)Target)->SetWrapEnabled(true, Value.AsSize());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIText*>(Text);
	} },

	DynamicProperty{ .Name = "color",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetColor(Value.AsVec3());
		((UIButton*)Target)->SetHoveredColor(Value.AsVec3() * 0.75f);
		((UIButton*)Target)->SetKeyboardHoveredColor(Value.AsVec3() * 0.75f);
		((UIButton*)Target)->SetPressedColor(Value.AsVec3() * 0.5f);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "normalColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "hoveredColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetHoveredColor(Value.AsVec3());
		((UIButton*)Target)->SetKeyboardHoveredColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "mouseHoveredColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetHoveredColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "keyboardHoveredColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetKeyboardHoveredColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "pressedColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIButton*)Target)->SetPressedColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIButton*>(Text);
	} },

	DynamicProperty{ .Name = "color",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "opacity",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetOpacity(Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "image",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetUseTexture(true, std::string(Value));
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "corners",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->GetRenderState()->CornerRadius = Value.AsSize();
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "border",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->GetRenderState()->BorderRadius = Value.AsSize();
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "borderColor",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetBorderColor(Value.AsVec3());
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "topLeftCorner",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetCornerVisible(1, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "topRightCorner",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetCornerVisible(3, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "bottomLeftCorner",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetCornerVisible(0, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "bottomRightCorner",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetCornerVisible(2, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "topBorder",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetBorderVisible(2, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "bottomBorder",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetBorderVisible(3, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "leftBorder",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetBorderVisible(1, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "rightBorder",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		((UIBackground*)Target)->SetBorderVisible(0, Value);
	},
	.IsTarget = [](kui::UIBox* Text) {
		return dynamic_cast<UIBackground*>(Text);
	} },

	DynamicProperty{ .Name = "width",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinWidth(Value.AsSize());
		Target->SetMaxWidth(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "height",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinHeight(Value.AsSize());
		Target->SetMaxHeight(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "size",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinSize(Value.AsSizeVec());
		Target->SetMaxSize(Value.AsSizeVec());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "minSize",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinSize(Value.AsSizeVec());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "maxSize",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinSize(Value.AsSizeVec());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "minWidth",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinWidth(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "minHeight",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMinHeight(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "maxWidth",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMaxWidth(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "maxHeight",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetMaxHeight(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "position",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetPosition(Value.AsVec2());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "allAlign",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetHorizontalAlign(ConvertAlign(Value));
		Target->SetVerticalAlign(ConvertAlign(Value));
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "horizontalAlign",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetHorizontalAlign(ConvertAlign(Value));
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "verticalAlign",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetVerticalAlign(ConvertAlign(Value));
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "padding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "upPadding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetUpPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "downPadding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetDownPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "leftPadding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetLeftPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "rightPadding",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetRightPadding(Value.AsSize());
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "hasMouseCollision",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->HasMouseCollision = Value;
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },

	DynamicProperty{ .Name = "orientation",
	.SetValue = [](kui::UIBox* Target, kui::AnyContainer Value) {
		Target->SetHorizontal(std::string(Value) == "horizontal");
	},
	.IsTarget = [](kui::UIBox*) {
		return true;
	} },
};
