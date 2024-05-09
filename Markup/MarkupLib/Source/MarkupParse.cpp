#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <iostream>
#include <filesystem>
#include <map>
using namespace KlemmUI;

std::vector<MarkupStructure::MarkupElement> MarkupParse::ParseFiles(std::vector<FileEntry> Files)
{
	std::map<std::string, std::vector<StringParse::Line>> FileLines;

	std::vector<ParsedElement> AllElements;

	for (const FileEntry& File : Files)
	{
		FileLines.insert({ File.Name, StringParse::SeparateString(File.Content) });
	}

	for (auto& File : FileLines)
	{
		ParseError::SetCode(File.second);

		auto Elements = GetElementsInFile(File.second, File.first);

		AllElements.reserve(Elements.size() + AllElements.size());

		for (const ParsedElement& Element : Elements)
		{
			AllElements.push_back(Element);
		}
	}

	std::vector<MarkupStructure::MarkupElement> StructureElements;
	for (auto& Element : AllElements)
	{
		auto& Lines = FileLines[Element.File];
		ParseError::SetCode(Lines);

		StructureElements.push_back(ParseElement(Element, Lines));
	}

	return StructureElements;
}

std::vector<MarkupParse::ParsedElement> MarkupParse::GetElementsInFile(std::vector<StringParse::Line>& Lines, std::string FileName)
{
	std::vector<ParsedElement> Elements;
	ParsedElement* Current = nullptr;
	size_t Depth = 0;
	for (size_t i = 0; i < Lines.size(); i++)
	{
		StringParse::Line& ln = Lines[i];
		ParseError::SetLine(i);

		std::string Content = ln.Peek();

		if (Content == "element" && Depth == 0)
		{
			ln.Get(); // element
			std::string Name = ln.Get();

			if (MarkupStructure::UIElement::IsDefaultElement(Name))
			{
				ParseError::Error("Invalid name: '" + Name + "'. A default element with this name already exists.");
				continue;
			}

			Elements.push_back(ParsedElement{
				.Name = Name,
				.File = FileName,
				.Start = i,
				.StartLine = ln.Index,
				});
			Current = &Elements[Elements.size() - 1];
			if (ln.Get() != "{")
			{
				ParseError::Error("Expected a '{' after 'element " + Current->Name + "'");
				Current = nullptr;
				Elements.pop_back();
				continue;
			}
			Depth++;
		}
		else if (ln.Contains("{"))
		{
			Depth++;
		}
		if (ln.Contains("}"))
		{
			if (Depth == 0)
			{
				ParseError::Error("Unexpected '}'");
				continue;
			}
			Depth--;
		}
	}

	if (Depth != 0)
	{
		ParseError::Error("Expected a closing '}'");
	}

	return Elements;
}

MarkupStructure::MarkupElement KlemmUI::MarkupParse::ParseElement(ParsedElement& Elem, std::vector<StringParse::Line>& Lines)
{
	MarkupStructure::MarkupElement Element;
	MarkupStructure::UIElement Root;
	Root.TypeName = Elem.Name;
	Root.Type = MarkupStructure::UIElement::ElementType::UserDefined;
	ParseScope(Root, Lines, Elem.Start, true);
	Element.Root = Root;
	Element.File = Elem.File;
	return Element;
}

void KlemmUI::MarkupParse::ParseScope(MarkupStructure::UIElement& Elem, std::vector<StringParse::Line> Lines, size_t Start, bool IsRoot)
{
	size_t Depth = 0;
	for (size_t i = Start + 1; i < Lines.size(); i++)
	{
		StringParse::Line& ln = Lines[i];
		ln.StringPos = 0;
		ParseError::SetLine(i);

		std::string Begin = ln.Get();

		if (Begin == "}")
		{
			if (Depth == 0)
			{
				return;
			}
			Depth--;
			continue;
		}
		if (Depth != 0)
		{
			if (ln.Contains("{"))
			{
				Depth++;
			}
			continue;
		}


		if (Begin == "{")
		{
			ParseError::Error("Unexpected '{'");
		}
		else if (Begin == "var")
		{
			if (!IsRoot)
			{
				ParseError::Error("Cannot declare variable here.");
			}
			std::string VariableName = ln.Get();
			std::string Value;

			if (ln.Get() == "=")
			{
				Value = ln.GetUntil("");
			}
			Elem.Variables.insert({ VariableName, MarkupStructure::UIElement::Variable(Value, {})});
		}
		else if (Begin == "child")
		{
			MarkupStructure::UIElement New;
			std::string Type = ln.Get();

			New.TypeName = Type;
			New.Type = MarkupStructure::UIElement::IsDefaultElement(New.TypeName) ? MarkupStructure::UIElement::ElementType::Default : MarkupStructure::UIElement::ElementType::UserDefined;
			std::string Next = ln.Get();
			ParseScope(New, Lines, i, false);

			// No element name.
			if (Next == "{")
			{
				Elem.Children.push_back(New);
			}
			else
			{
				New.ElementName = Next;
				if (ln.Get() != "{")
				{
					ParseError::Error("Expected a '{'");
				}
				Elem.Children.push_back(New);
			}
		}
		else
		{
			MarkupStructure::Property p{
				.Name = Begin,
			};

			if (ln.Get() != "=")
			{
				ParseError::Error("Expected '=' after '" + ln.Previous() + "'");
				continue;
			}
			p.Value = ln.GetUntil("");

			Elem.ElementProperties.push_back(p);
		}
		if (ln.Contains("{"))
		{
			Depth++;
		}
	}
}
