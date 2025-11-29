#pragma once
#include "MarkupStructure.h"
#include "StringParse.h"
#include <cstdint>

namespace kui::markup
{
	struct FileEntry
	{
		std::string Content;
		std::string Name;
		std::string Path;
	};

	markup::ParseResult ParseFiles(std::vector<FileEntry> Files);

	struct ParsedElement
	{
		std::string Name;
		std::string File;
		std::string FilePath;
		size_t Start;
		size_t StartLine;
		size_t End;
		stringParse::StringToken DefinitionToken;
		markup::MarkupElement StructureElement;
	};

	struct FileResult
	{
		std::vector<ParsedElement> Elements;
		std::vector<markup::Constant> Constants;
		std::vector<markup::Global> Globals;
	};

	FileResult ReadFile(std::vector<stringParse::Line>& Lines, std::string FileName, std::string FilePath = "");
	markup::MarkupElement ParseElement(ParsedElement& Elem, std::vector<stringParse::Line>& Lines);

	// Returns the last token that belongs to the element.
	stringParse::StringToken ParseScope(markup::UIElement& Elem, std::vector<stringParse::Line> Lines, size_t Start, bool IsRoot);
}