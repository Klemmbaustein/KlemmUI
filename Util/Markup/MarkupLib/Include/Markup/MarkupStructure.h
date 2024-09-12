#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "StringParse.h"

namespace KlemmUI::MarkupStructure
{
	struct MarkupElement;
	struct ParseResult;

	struct Property
	{
		StringParse::StringToken Name;
		std::string Value;
	};

	enum class PropElementType
	{
		UIBox,
		Backgrounds_Begin,
		UIBackground,
		UIButton,
		UITextField,
		Backgrounds_End,
		UIScrollBox,
		UIText,
		Unknown,
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
		StringParse::StringToken TypeName;
		std::string ElementName;
		ElementType Type = ElementType::Default;

		std::vector<UIElement> Children;
		std::vector<Property> ElementProperties;
		size_t StartChar = 0, StartLine = 0, EndChar = 0, EndLine = 0;

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

	PropElementType GetTypeFromString(std::string TypeName);
	std::string GetStringFromType(PropElementType Type);
	bool IsSubclassOf(PropElementType Class, PropElementType Parent);

	struct PropertyElement
	{
		PropElementType Type;
		std::string Name;
		std::string Description;
		std::vector<std::string> SetFormat;
		// If the VarType is Size, then a second function needs to be specified that sets the size mode.
		std::string SetSizeFormat;
		std::string (*CreateCodeFunction)(std::string InValue) = nullptr;
		UIElement::Variable::VariableType VarType;
		bool AlwaysSet = false;
		std::string Default;
	};
	extern std::vector<PropertyElement> Properties;

	struct MarkupElement
	{
		UIElement Root;
		std::string File;
		StringParse::StringToken FromToken;

		void WriteHeader(const std::string& Path, ParseResult& MarkupElements);
	private:
		std::string MakeConstructor(ParseResult& MarkupElements);
	};

	struct ParseResult
	{
		std::vector<MarkupElement> Elements;
		std::vector<Constant> Constants;
		std::map<std::string, std::vector<StringParse::Line>> FileLines;

		Constant* GetConstant(std::string Name);
	};
}