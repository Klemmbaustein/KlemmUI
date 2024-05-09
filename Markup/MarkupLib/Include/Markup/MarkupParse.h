#pragma once
#include "MarkupStructure.h"
#include "StringParse.h"

namespace KlemmUI::MarkupParse
{
	std::vector<MarkupStructure::MarkupElement> ParseFiles(std::vector<std::string> FileStrings);

	struct ParsedElement
	{
		std::string Name;
		std::string File;
		size_t Start;
		size_t End;
	};

	std::vector<ParsedElement> GetElementsInFile(std::vector<StringParse::Line>& Lines, std::string FileName);
}