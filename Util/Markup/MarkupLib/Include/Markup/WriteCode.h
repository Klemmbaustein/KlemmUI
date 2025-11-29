#pragma once
#include <Markup/MarkupStructure.h>
#include <fstream>

namespace kui::markup
{
	class ElementWriter
	{
	public:

		ElementWriter(MarkupElement* ToWrite, ParseResult* Parsed);

		void Write(std::ofstream& Stream);
		void GetDependencies(std::set<std::string>& OutDependencies);
		void GetElementDependencies(std::set<std::string>& OutDependencies);

	private:

		void WriteConstructor(std::ostream& Stream);
		void WriteElementConstructor(std::ostream& Stream, UIElement& Target, size_t Depth, bool IsThis, std::string Parent);

		void WriteProperty(stringParse::StringToken Value, PropertyElement& FoundProperty,
			std::ostream& Stream, UIElement& Target, size_t Depth);
		void WriteProperties(std::ostream& Stream, UIElement& Target, size_t Depth);

		void GetDependenciesForElement(std::set<std::string>& OutDependencies, UIElement* Element);
		void GetElementDependenciesForElement(std::set<std::string>& OutDependencies, UIElement* Element);

		static std::string RepeatedTabs(size_t Length);

		static std::string ToCppTypeName(stringParse::StringToken Name);

		static bool IsNameUnnamed(std::string Name);

		void WriteVariableSetter(std::ostream& Stream, UIElement::Variable& Var);

		void WriteOnGlobalChanged(std::ostream& Stream);
		void WriteOnTranslationChanged(std::ostream& Stream);

		MarkupElement* ToWrite = nullptr;
		size_t UnnamedCounter = 0;
		ParseResult* Parsed;
		std::map<std::string, UIElement*> Elements;
	};
}