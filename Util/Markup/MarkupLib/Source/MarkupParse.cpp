#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <iostream>
#include <filesystem>
#include <map>
using namespace kui;

MarkupStructure::ParseResult MarkupParse::ParseFiles(std::vector<FileEntry> Files)
{
	std::map<std::string, std::vector<stringParse::Line>> FileLines;

	std::vector<ParsedElement> AllElements;

	for (const FileEntry& File : Files)
	{
		FileLines.insert({ File.Name, stringParse::SeparateString(File.Content) });
	}

	std::vector<MarkupStructure::Constant> Consts;
	std::vector<MarkupStructure::Global> Globals;

	for (auto& File : FileLines)
	{
		parseError::SetCode(File.second, File.first);

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

		for (const MarkupStructure::Global& g : FileContent.Globals)
		{
			Globals.push_back(g);
		}
	}

	std::vector<MarkupStructure::MarkupElement> StructureElements;
	for (auto& Element : AllElements)
	{
		auto& Lines = FileLines[Element.File];
		parseError::SetCode(Lines, Element.File);

		StructureElements.push_back(ParseElement(Element, Lines));
	}

	return MarkupStructure::ParseResult
	{
		.Elements = StructureElements,
		.Constants = Consts,
		.Globals = Globals,
		.FileLines = FileLines,
	};
}

MarkupParse::FileResult MarkupParse::ReadFile(std::vector<stringParse::Line>& Lines, std::string FileName)
{
	FileResult Out;
	ParsedElement* Current = nullptr;
	size_t Depth = 0;
	for (size_t i = 0; i < Lines.size(); i++)
	{
		stringParse::Line& ln = Lines[i];
		ln.ResetPos();
		parseError::SetLine(i);

		stringParse::StringToken Content = ln.Peek();

		if (Content == "element" && Depth == 0)
		{
			ln.Get(); // element
			stringParse::StringToken Name = ln.Get();
			if (MarkupStructure::UIElement::IsDefaultElement(Name))
			{
				parseError::Error("Invalid name: '" + Name.Text + "'. A default element with this name already exists.", Name);
				if (ln.Contains("{"))
					Depth++;
				continue;
			}

			if (!Name.IsName())
			{
				parseError::Error("Invalid name for element: '" + Name.Text + "'", Name);
				if (ln.Contains("{"))
					Depth++;
				continue;
			}

			Out.Elements.push_back(ParsedElement{
				.Name = Name,
				.File = FileName,
				.Start = i,
				.StartLine = ln.Index,
				.DefinitionToken = Name
				});
			Current = &Out.Elements[Out.Elements.size() - 1];
			if (ln.Get() != "{")
			{
				parseError::Error("Expected a '{' after 'element " + Current->Name + "', got '" + ln.Previous().Text + "'", ln.Previous());
				Current = nullptr;
				Out.Elements.pop_back();
				continue;
			}
			Depth++;
		}
		else if (Content == "const" || Content == "global")
		{
			bool IsConst = ln.Get() == "const";
			stringParse::StringToken Name = ln.Get();

			if (ln.Get() != "=" && IsConst)
			{
				parseError::Error(ln.Peek().Empty() ? "Expected a '=' after a const value." : "Unexpected '" + ln.Peek().Text + "' after a const value. Expected '='", ln.Peek());
				continue;
			}

			if (IsConst)
			{
				Out.Constants.push_back(MarkupStructure::Constant{
					.Name = Name,
					.Value = ln.GetUntil(""),
					.File = FileName
					});
			}
			else
			{
				Out.Globals.push_back(MarkupStructure::Global{
					.Name = Name,
					.Value = ln.GetUntil(""),
					.File = FileName
					});
			}
		}
		else if (ln.Contains("{") && Depth != 0)
		{
			Depth++;
		}
		else if (ln.Contains("}"))
		{
			if (Depth == 0)
			{
				parseError::Error("Unexpected '}'", Content);
				continue;
			}
			Depth--;
		}
		else if (Depth == 0)
		{
			parseError::Error("Unexpected '" + ln.Peek().Text + "'", ln.Peek());
		}
	}

	if (Depth != 0)
	{
		parseError::Error("Expected a closing '}'", stringParse::StringToken("", 0, 10000));
	}

	return Out;
}

MarkupStructure::MarkupElement kui::MarkupParse::ParseElement(ParsedElement& Elem, std::vector<stringParse::Line>& Lines)
{
	MarkupStructure::MarkupElement Element;
	MarkupStructure::UIElement Root;
	Root.TypeName = Elem.DefinitionToken;
	Root.Type = MarkupStructure::UIElement::ElementType::UserDefined;
	
	Lines.at(Elem.Start).StringPos = 0;
	auto StartToken = Lines.at(Elem.Start).Peek();

	auto EndToken = ParseScope(Root, Lines, Elem.Start, true);

	Root.StartChar = StartToken.BeginChar;
	Root.StartLine = StartToken.Line;
	Root.EndChar = EndToken.EndChar;
	Root.EndLine = EndToken.Line;

	Element.Root = Root;
	Element.FromToken = Elem.DefinitionToken;
	Element.File = Elem.File;
	return Element;
}

stringParse::StringToken kui::MarkupParse::ParseScope(MarkupStructure::UIElement& Elem, std::vector<stringParse::Line> Lines, size_t Start, bool IsRoot)
{
	size_t Depth = 0;
	for (size_t i = Start + 1; i < Lines.size(); i++)
	{
		stringParse::Line& ln = Lines[i];
		parseError::SetLine(i);
		ln.ResetPos();

		stringParse::StringToken Begin = ln.Get();

		if (Begin == "}")
		{
			if (Depth == 0)
			{
				return Begin;
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
			parseError::Error("Unexpected '{'", Begin);
		}
		else if (Begin == "var")
		{
			if (!IsRoot)
			{
				parseError::Error("Cannot declare variable here.", Begin);
			}
			std::string VariableName = ln.Get();
			stringParse::StringToken Value;

			if (ln.Get() == "=")
			{
				Value = ln.GetUntil("");
			}
			Elem.Variables.insert({ VariableName, MarkupStructure::UIElement::Variable(Value, {})});
		}
		else if (Begin == "child")
		{
			MarkupStructure::UIElement New;
			stringParse::StringToken Type = ln.Get();

			New.TypeName = Type;
			New.Type = MarkupStructure::UIElement::IsDefaultElement(New.TypeName)
				? MarkupStructure::UIElement::ElementType::Default
				: MarkupStructure::UIElement::ElementType::UserDefined;

			stringParse::StringToken Next = ln.Get();
			auto EndToken = ParseScope(New, Lines, i, false);

			New.StartChar = Next.BeginChar;
			New.StartLine = Next.Line;

			New.EndChar = EndToken.EndChar;
			New.EndLine = EndToken.Line;

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
					parseError::Error("Unexpected '" + ln.Previous().Text + "'", ln.Previous());
				}
			}
		}
		else
		{
			MarkupStructure::Property p{
				.Name = Begin,
			};

			if (ln.Peek() != "=")
			{
				parseError::Error("Expected '=' after '" + ln.Previous().Text + "', got '" + ln.Peek().Text + "'", ln.Peek());
				continue;
			}
			ln.Get();

			p.Value = ln.GetUntil("");

			Elem.ElementProperties.push_back(p);
		}
		if (ln.Contains("{"))
		{
			Depth++;
		}
	}
	return stringParse::StringToken();
}
