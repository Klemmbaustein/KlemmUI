#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>

namespace KlemmUI::MarkupStructure
{
	struct MarkupElement;
	struct ParseResult;

	struct Property
	{
		std::string Name;
		std::string Value;
	};

	struct UIElement
	{
		static bool IsDefaultElement(const std::string& Name);
		
		enum class ElementType
		{
			/// UIBox, UIBackground, ...
			Default,
			/// Any element defined with `element xy {}`
			UserDefined,
		};
		/// Name of the element's type (UIBox, UIButton, MyElement...)
		std::string TypeName;
		std::string ElementName;
		ElementType Type = ElementType::Default;

		std::vector<UIElement> Children;
		std::vector<Property> ElementProperties;

		struct Variable
		{
			std::string Value;
			std::vector<std::string> References;

			enum class VariableType
			{
				None,
				Number,
				String,
				Size,
				SizeNumber,
				SizeMode,
				Align,
				Vector3,
				Vector2,
				Bool,
				Orientation,
				Callback,
				CallbackIndex,
			};

			struct VariableTypeDescription
			{
				std::string Name;
				std::string CppName;
			};

			static std::map<VariableType, VariableTypeDescription> Descriptions;

			VariableType Type;
		};
		std::map<std::string, Variable> Variables;
		std::string WriteVariableSetter(std::pair<std::string, Variable> Var);
		std::set<std::string> GetElementDependencies() const;
		std::set<std::string> GetNamedElements() const;

		std::string MakeCode(std::string Parent, UIElement* Root, size_t& Depth, ParseResult& MarkupElements);
	};

	struct Constant
	{
		std::string Name;
		std::string Value;
	};

	struct MarkupElement
	{
		UIElement Root;
		std::string File;

		void WriteHeader(const std::string& Path, ParseResult& MarkupElements);
	private:
		std::string MakeConstructor(ParseResult& MarkupElements);
	};

	struct ParseResult
	{
		std::vector<MarkupElement> Elements;
		std::vector<Constant> Constants;

		Constant* GetConstant(std::string Name);
	};
}