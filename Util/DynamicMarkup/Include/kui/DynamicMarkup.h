#pragma once
#include <kui/UI/UIBackground.h>
#include <kui/Markup/MarkupBox.h>
#include <kui/Markup/Markup.h>

namespace kui::MarkupStructure
{
	class MarkupElement;
	class ParseResult;
}

namespace kui::markup
{
	struct MarkupFile
	{
		std::string Name;
		std::string Content;
	};

	class DynamicMarkupContext
	{
	public:

		DynamicMarkupContext();
		~DynamicMarkupContext();

		void LoadFilesFromPath(std::string Path);
		void LoadFiles(std::vector<MarkupFile> Files);

		MarkupStructure::ParseResult* Parsed = nullptr;
	};

	class UIDynMarkupBox : public UIBox, public kui::markup::MarkupBox
	{
	public:

		struct VariableValue
		{
			UIBox* Box;
			std::string Name;
			std::string Value;
		};

		struct VariableInfo
		{
			kui::AnyContainer Value;
			std::map<UIBox*, VariableValue> Elements;
		};

		void OnTranslationChanged() override;
		void OnGlobalChanged() override;

		UIDynMarkupBox(DynamicMarkupContext* Context, std::string ClassName);
		virtual ~UIDynMarkupBox() override;
		UIDynMarkupBox(const UIDynMarkupBox&) = delete;

		std::map<UIBox*, VariableValue> Globals;
		std::map<UIBox*, VariableValue> Translated;

		std::map<std::string, VariableInfo> Variables;
		std::map<std::string, UIBox*> NamedChildren;

		void SetVariable(std::string Name, kui::AnyContainer Value);

		AnyContainer GetVariable(std::string Name, kui::AnyContainer Value);
		MarkupStructure::MarkupElement* Element = nullptr;

	private:
		DynamicMarkupContext* Context = nullptr;
		std::string ElementName;
		void LoadFromElement(MarkupStructure::MarkupElement* From);
	};
}