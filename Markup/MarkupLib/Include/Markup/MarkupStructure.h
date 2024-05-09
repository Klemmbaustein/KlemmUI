#pragma once
#include <string>
#include <vector>
#include <set>

namespace KlemmUI::MarkupStructure
{
	struct Property
	{
		std::string Name;
		std::string Value;
	};

	struct UIElement
	{
		enum class ElementType
		{
			/// UIBox, UIBackground, ...
			Default,
			/// Any element defined with `element xy()`
			UserDefined,
		};
		/// Name of the element's type (UIBox, UIButton, MyElement...)
		std::string TypeName;
		std::string ElementName;
		ElementType Type = ElementType::Default;

		std::vector<UIElement> Children;
		std::vector<Property> ElementProperties;
		std::set<std::string> GetElementDependencies() const;

		std::string MakeCode(std::string Parent, size_t Depth) const;

		std::string WriteElementProperty(std::string ElementName, const Property& p) const;
	};

	struct MarkupElement
	{
		UIElement Root;
		std::string File;

		void WriteHeader(const std::string& Path);
	private:
		std::string MakeConstructor() const;
	};
}