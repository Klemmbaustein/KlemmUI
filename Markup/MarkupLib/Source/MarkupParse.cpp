#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
using namespace KlemmUI;

std::vector<MarkupStructure::MarkupElement> MarkupParse::ParseFiles(std::vector<std::string> FileStrings)
{
	std::vector<StringParse::Line> Lines = StringParse::SeparateString(FileStrings[0]);

	auto Elements = GetElementsInFile(Lines, "test.kui");

	return {};
}

std::vector<MarkupParse::ParsedElement> MarkupParse::GetElementsInFile(std::vector<StringParse::Line>& Lines, std::string FileName)
{
	std::vector<ParsedElement> Elements;
	ParsedElement* Current = nullptr;
	for (size_t i = 0; i < Lines.size(); i++)
	{
		StringParse::Line& ln = Lines[i];

		std::string Content = ln.Get();

		if (Content == "element")
		{
			Elements.push_back(ParsedElement{
				.Name = ln.Get(),
				.File = FileName,
				.Start = i,
				});
		}
	}
	return Elements;
}
