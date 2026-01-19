#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <map>
using namespace kui;

markup::ParseResult markup::ParseFiles(std::vector<FileEntry> Files, ParseOptions* Options)
{
	std::map<std::string, FileData> OutFiles;

	std::vector<ParsedElement> AllElements;

	for (const FileEntry& File : Files)
	{
		OutFiles.insert({ File.Name, {
			File.Path,
			Options && Options->Tokenize
			? Options->Tokenize(File.Content, File.Name)
			: stringParse::SeparateString(File.Content)}
			});
	}

	std::vector<markup::Constant> Consts;
	std::vector<markup::Global> Globals;

	for (auto& File : OutFiles)
	{
		parseError::SetCode(File.second.Lines, File.first);

		auto FileContent = ReadFile(File.second.Lines, File.first, File.second.FilePath);

		AllElements.reserve(FileContent.Elements.size() + AllElements.size());

		for (const ParsedElement& Element : FileContent.Elements)
		{
			AllElements.push_back(Element);
		}

		for (const markup::Constant& Const : FileContent.Constants)
		{
			Consts.push_back(Const);
		}

		for (const markup::Global& g : FileContent.Globals)
		{
			Globals.push_back(g);
		}
	}

	std::vector<markup::MarkupElement> StructureElements;
	for (auto& Element : AllElements)
	{
		auto& Data = OutFiles[Element.File];
		parseError::SetCode(Data.Lines, Element.File);

		StructureElements.push_back(ParseElement(Element, Data.Lines, Options));
	}

	return markup::ParseResult
	{
		.Elements = StructureElements,
		.Constants = Consts,
		.Globals = Globals,
		.Data = OutFiles,
	};
}

markup::FileResult markup::ReadFile(std::vector<stringParse::Line>& Lines,
	std::string FileName, std::string FilePath)
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
			if (markup::UIElement::IsDefaultElement(Name))
			{
				parseError::Error("Invalid name: '" + Name.Text
					+ "'. A default element with this name already exists.", Name);
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
				.FilePath = FilePath,
				.Start = i,
				.StartLine = ln.Index,
				.DefinitionToken = Name,
				});
			Current = &Out.Elements[Out.Elements.size() - 1];
			if (ln.Get() != "{")
			{
				parseError::Error("Expected a '{' after 'element " + Current->Name
					+ "', got '" + ln.Previous().Text + "'", ln.Previous());
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
				parseError::Error(ln.Peek().Empty() ? "Expected a '=' after a const value." : "Unexpected '"
					+ ln.Peek().Text + "' after a const value. Expected '='", ln.Peek());
				continue;
			}

			if (IsConst)
			{
				Out.Constants.push_back(markup::Constant{
					.Name = Name,
					.Value = ln.GetUntil(""),
					.File = FileName
					});
			}
			else
			{
				Out.Globals.push_back(markup::Global{
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

markup::MarkupElement kui::markup::ParseElement(ParsedElement& Elem,
	std::vector<stringParse::Line>& Lines, ParseOptions* Options)
{
	markup::MarkupElement Element;
	markup::UIElement Root;
	Element.FilePath = Elem.FilePath;
	Root.TypeName = Elem.DefinitionToken;
	Root.Type = markup::UIElement::ElementType::UserDefined;

	Lines.at(Elem.Start).StringPos = 0;
	auto StartToken = Lines.at(Elem.Start).Peek();

	auto EndToken = ParseScope(Root, Lines, Elem.Start, &Element, Options);

	Root.StartChar = StartToken.BeginChar;
	Root.StartLine = StartToken.Line;
	Root.EndChar = EndToken.EndChar;
	Root.EndLine = EndToken.Line;

	Element.Root = Root;
	Element.FromToken = Elem.DefinitionToken;
	Element.File = Elem.File;
	return Element;
}

void kui::markup::ParseCustomData(std::string Name, MarkupElement& Elem, std::vector<stringParse::Line>& Lines, size_t Start)
{
	auto& NewCustom = Elem.CustomSegments.insert({ Name, {} }).first->second;
	NewCustom.StartLine = Lines[Start].Previous().Line;
	NewCustom.StartChar = Lines[Start].Previous().EndChar;

	size_t Depth = 0;
	for (size_t i = Start + 1; i < Lines.size(); i++)
	{
		stringParse::Line& ln = Lines[i];
		ln.ResetPos();

		if (ln.Peek() == "}")
		{
			if (Depth == 0)
			{
				auto Found = ln.Peek();
				NewCustom.EndChar = Found.BeginChar;
				NewCustom.EndLine = Found.Line;
				return;
			}
			Depth--;
		}
		if (ln.Contains("{"))
		{
			Depth++;
		}
		NewCustom.Lines.push_back(ln);
	}
}

stringParse::StringToken kui::markup::ParseScope(markup::UIElement& Elem, std::vector<stringParse::Line>& Lines,
	size_t Start, markup::MarkupElement* RootElement, ParseOptions* Options)
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
			if (!RootElement)
			{
				parseError::Error("Cannot declare variable here.", Begin);
			}
			stringParse::StringToken VariableName = ln.Get();
			stringParse::StringToken Value;

			if (ln.Get() == "=")
			{
				Value = ln.GetUntil("");
			}
			Elem.Variables.insert({ VariableName, UIElement::Variable(Value, {}, VariableName) });
		}
		else if (Begin == "child")
		{
			UIElement New;
			stringParse::StringToken Type = ln.Get();

			New.TypeName = Type;
			New.Type = UIElement::IsDefaultElement(New.TypeName)
				? UIElement::ElementType::Default
				: UIElement::ElementType::UserDefined;

			stringParse::StringToken Next = ln.Get();
			auto EndToken = ParseScope(New, Lines, i, nullptr, Options);

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
		else if (Options && Options->CustomFields.contains(Begin) && RootElement)
		{
			ParseCustomData(Begin, *RootElement, Lines, i);
		}
		else
		{
			Property p
			{
				.Name = Begin,
			};

			if (ln.Peek() != "=")
			{
				parseError::Error("Expected '=' after '"
					+ ln.Previous().Text + "', got '" + ln.Peek().Text + "'", ln.Peek());
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
