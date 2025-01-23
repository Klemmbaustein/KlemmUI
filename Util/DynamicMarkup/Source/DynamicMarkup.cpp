#include <kui/DynamicMarkup.h>
#include <Markup/MarkupParse.h>
#include <Markup/MarkupVerify.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Values.h"
#include <kui/App.h>
using namespace kui;
using namespace kui::MarkupParse;
using namespace kui::MarkupStructure;

static MarkupElement* GetElementFromName(std::string Name, markup::DynamicMarkupContext* From)
{
	for (auto& elem : From->Parsed->Elements)
	{
		if (elem.FromToken.Text == Name)
		{
			return &elem;
		}
	}
	return nullptr;
}

void kui::markup::DynamicMarkupContext::LoadFilesFromPath(std::string Path)
{
	std::vector<FileEntry> MarkupFiles;
	for (auto& i : std::filesystem::directory_iterator(Path))
	{
		std::ifstream Source = std::ifstream(i.path());
		std::stringstream SourceStream;
		SourceStream << Source.rdbuf();

		MarkupFiles.push_back(FileEntry{
			.Content = SourceStream.str(),
			.Name = i.path().filename().string(),
			});
	}
	Parsed = new ParseResult(ParseFiles(MarkupFiles));
	markupVerify::Verify(*Parsed);
}

void kui::markup::DynamicMarkupContext::LoadFiles(std::vector<MarkupFile> Files)
{
	std::vector<FileEntry> MarkupFiles;

	for (auto& i : Files)
	{
		MarkupFiles.push_back(FileEntry{
			.Content = i.Content,
			.Name = i.Name
			});
	}

	Parsed = new ParseResult(ParseFiles(MarkupFiles));
	markupVerify::Verify(*Parsed);
}

kui::markup::DynamicMarkupContext::DynamicMarkupContext()
{

}
kui::markup::DynamicMarkupContext::~DynamicMarkupContext()
{
	if (Parsed)
		delete Parsed;
}

kui::markup::UIDynMarkupBox::UIDynMarkupBox(DynamicMarkupContext* Context, std::string ClassName)
	: UIBox(true)
{
	this->Context = Context;
	ElementName = ClassName;
	MarkupElement* Elem = GetElementFromName(ClassName, Context);
	if (Elem)
	{
		LoadFromElement(Elem);
		return;
	}
	std::cerr << "No such element: " << ClassName << std::endl;
}

void kui::markup::UIDynMarkupBox::OnTranslationChanged()
{

}

void kui::markup::UIDynMarkupBox::SetVariable(std::string Name, kui::AnyContainer Value)
{
	MarkupElement* Elem = GetElementFromName(ElementName, Context);

	if (!Elem)
		return;
	Variables[Name].Value = Value;
	for (auto& i : Variables[Name].Elements)
	{
		SetUIBoxValue(i.second.Box, i.second.Name, i.second.Value, Context, this, *Elem);
	}
}

void kui::markup::UIDynMarkupBox::OnGlobalChanged()
{
	MarkupElement* Elem = GetElementFromName(ElementName, Context);

	if (!Elem)
		return;

	for (auto& i : Globals)
	{
		SetUIBoxValue(i.second.Box, i.second.Name, i.second.Value, Context, this, *Elem);
	}
}

kui::markup::UIDynMarkupBox::~UIDynMarkupBox()
{
}

void kui::markup::UIDynMarkupBox::LoadFromElement(MarkupStructure::MarkupElement* From)
{
	this->Element = From;
	ApplyElementValues(this, *From, Context, this);
}
