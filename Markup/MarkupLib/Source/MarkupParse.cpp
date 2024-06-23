#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <iostream>
#include <filesystem>
#include <map>
using namespace KlemmUI;

MarkupStructure::ParseResult MarkupParse::ParseFiles(std::vector<FileEntry> Files)
{
	std::map<std::string, std::vector<StringParse::Line>> FileLines;

	std::vector<ParsedElement> AllElements;

	for (const FileEntry& File : Files)
	{
		FileLines.insert({ File.Name, StringParse::SeparateString(File.Content) });
	}

	std::vector<MarkupStructure::Constant> Consts;

	for (auto& File : FileLines)
	{
		ParseError::SetCode(File.second, File.first);

		auto FileContent = ReadFile(File.second, File.first);

		AllElements.reserve(FileContent.Elements.size() + AllElements.size());

		for (const ParsedElement& Element : FileContent.Elements)
		{
			AllElements.push_back(Element);
		}

		for (const MarkupStructure::Constant& Const : FileContent.Constants)
		{
			Consts.push_back(Const);
		}
	}

	std::vector<MarkupStructure::MarkupElement> StructureElements;
	for (auto& Element : AllElements)
	{
		auto& Lines = FileLines[Element.File];
		ParseError::SetCode(Lines, Element.File);

		StructureElements.push_back(ParseElement(Element, Lines));
	}

	return MarkupStructure::ParseResult{
		.Elements = StructureElements,
		.Constants = Consts,
	};
}

MarkupParse::FileResult MarkupParse::ReadFile(std::vector<StringParse::Line>& Lines, std::string FileName)
{
	FileResult Out;
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

			Out.Elements.push_back(ParsedElement{
				.Name = Name,
				.File = FileName,
				.Start = i,
				.StartLine = ln.Index,
				});
			Current = &Out.Elements[Out.Elements.size() - 1];
			if (ln.Get() != "{")
			{
				ParseError::Error("Expected a '{' after 'element " + Current->Name + "'");
				Current = nullptr;
				Out.Elements.pop_back();
				continue;
			}
			Depth++;
		}
		else if (Content == "const")
		{
			ln.Get(); // const
			std::string Name = ln.Get();

			if (ln.Get() != "=")
			{
				ParseError::Error(ln.Previous().empty() ? "Expected a '=' after a constant definition." : "Unexpected '" + ln.Previous() + "' after a constant definition. Expected '='");
				continue;
			}

			Out.Constants.push_back(MarkupStructure::Constant{
				.Name = Name,
				.Value = ln.GetUntil(""),
				});
		}
		else if (ln.Contains("{"))
		{
			Depth++;
		}
		else if (ln.Contains("}"))
		{
			if (Depth == 0)
			{
				ParseError::Error("Unexpected '}'");
				continue;
			}
			Depth--;
		}
		else if (Depth == 0)
		{
			ParseError::Error("Unexpected '" + ln.Get() + "'");
		}
	}

	if (Depth != 0)
	{
		ParseError::Error("Expected a closing '}'");
	}

	return Out;
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
				Elem.Children.push_back(New);
				if (ln.Empty())
				{
					continue;
				}
				else if (ln.Get() != "{")
				{
					ParseError::Error("Unexpected '" + ln.Previous() + "'");
				}
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
