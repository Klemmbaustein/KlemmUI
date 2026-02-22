#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "StringParse.h"

namespace kui::markup
{
	struct MarkupElement;
	struct ParseResult;

	struct Property
	{
		stringParse::StringToken Name;
		stringParse::StringToken Value;
	};

	enum class PropElementType
	{
		UIBox,
		Backgrounds_Begin,
		UIBackground,
		UIButton,
		UITextField,
		UIBlurBackground,
		UISpinner,
		Backgrounds_End,
		UIScrollBox,
		UIText,
		Unknown,
	};

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
		Vec2,
		Bool,
		Orientation,
		Callback,
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
		stringParse::StringToken TypeName;
		stringParse::StringToken ElementName;
		std::string File;
		ElementType Type = ElementType::Default;

		std::vector<UIElement> Children;
		std::vector<Property> ElementProperties;
		size_t StartChar = 0, StartLine = 0, EndChar = 0, EndLine = 0;
		std::vector<Property> TranslatedProperties;
		std::vector<Property> GlobalProperties;

		struct Variable
		{
			std::string Value;
			std::vector<std::string> References;
			stringParse::StringToken Token;

			struct VariableTypeDescription
			{
				std::string Name;
				std::string CppName;
			};

			static std::map<VariableType, VariableTypeDescription> Descriptions;

			VariableType Type = VariableType::None;
		};
		std::map<std::string, Variable> Variables;
	};

	struct Constant
	{
		stringParse::StringToken Name;
		std::string Value;
		std::string File;
	};
	struct Global
	{
		stringParse::StringToken Name;
		VariableType Type = VariableType::None;
		std::string Value;
		std::string File;
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
		VariableType VarType;
		bool AlwaysSet = false;
		std::string Default;
	};
	extern std::vector<PropertyElement> Properties;

	struct CustomSegmentData
	{
		size_t StartChar = 0, StartLine = 0, EndChar = 0, EndLine = 0;
		std::vector<stringParse::Line> Lines;
	};

	struct MarkupElement
	{
		UIElement Root;
		std::string FilePath;
		std::string File;
		stringParse::StringToken Derived;
		stringParse::StringToken FromToken;

		std::map<std::string, CustomSegmentData> CustomSegments;
	};

	struct FileData
	{
		std::string FilePath;
		std::vector<stringParse::Line> Lines;
	};

	struct ParseResult
	{
		std::vector<MarkupElement> Elements;
		std::vector<Constant> Constants;
		std::vector<Global> Globals;
		std::map<std::string, FileData> Data;

		Constant* GetConstant(std::string Name);
		Global* GetGlobal(std::string Name);
		MarkupElement* GetElement(std::string Name);
	};
}