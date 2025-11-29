#pragma once
#include <kui/UI/UIBox.h>
#include <kui/Markup/Markup.h>
#include <Markup/MarkupStructure.h>

namespace kui::markup
{
	class DynamicMarkupContext;
	class UIDynMarkupBox;

	using namespace markup;

	UIBox* CreateNew(std::string Name, DynamicMarkupContext* From);
	void ApplyElementValues(UIBox* Box, MarkupElement& Target, DynamicMarkupContext* From, UIDynMarkupBox* Root);
	void ApplyElementValues(UIBox* Box, MarkupElement& TargetElement, UIElement Target, DynamicMarkupContext* From, UIDynMarkupBox* Root);

	void SetUIBoxValue(UIBox* Target, std::string Name, std::string Value, DynamicMarkupContext* From, UIDynMarkupBox* Root, MarkupElement& Element);

	kui::AnyContainer ToAny(std::string Value, std::function<std::string(std::string)> GetTranslated, markup::VariableType Type);
}