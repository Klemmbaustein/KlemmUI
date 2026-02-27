#pragma once
#include "MarkupStructure.h"
#include "StringParse.h"
#include <cstdint>
#include <functional>

namespace kui::markup
{
	struct FileEntry
	{
		std::string Content;
		std::string Name;
		std::string Path;
	};

	struct ParseOptions
	{
		std::set<std::string> CustomFields;
		std::function<std::vector<stringParse::Line>(const std::string& FileData, const std::string& File)> Tokenize;
	};

	markup::ParseResult ParseFiles(std::vector<FileEntry> Files, ParseOptions* Options = nullptr);

	struct ParsedElement
	{
		std::string Name;
		std::string File;
		std::string FilePath;
		size_t Start;
		size_t StartLine;
		size_t End;
		stringParse::StringToken DefinitionToken;
		stringParse::StringToken DerivedToken;
		markup::MarkupElement StructureElement;
		std::map<std::string, std::vector<stringParse::Line>> CustomFields;
	};

	struct FileResult
	{
		std::vector<ParsedElement> Elements;
		std::vector<markup::Constant> Constants;
		std::vector<markup::Global> Globals;
	};

	FileResult ReadFile(std::vector<stringParse::Line>& Lines, std::string FileName, std::string FilePath = "");
	markup::MarkupElement ParseElement(ParsedElement& Elem, std::vector<stringParse::Line>& Line, ParseOptions* Optionss);

	void ParseCustomData(std::string Name, MarkupElement& Elem, std::vector<stringParse::Line>& Lines, size_t Start);

	// Returns the last token that belongs to the element.
	stringParse::StringToken ParseScope(markup::UIElement& Elem, std::vector<stringParse::Line>& Lines,
		size_t Start, markup::MarkupElement* RootElement, ParseOptions* Options);
}