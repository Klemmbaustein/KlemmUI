#include <Markup/WriteHeader.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

void kui::writeHeader::WriteHeaders(std::string Dir, kui::MarkupStructure::ParseResult& From)
{
	for (auto& i : From.Elements)
	{
		if (i.File.empty())
			continue;
		i.Header = Dir + "/" + i.File.substr(0, i.File.find_last_of(".")) + ".kui.hpp";
	}

	std::set<std::string> ExistingHeaders;

	for (const auto& HeaderElement : From.Elements)
	{
		std::string CurrentHeader = HeaderElement.Header;
		if (HeaderElement.WrittenHeader)
			continue;
		std::stringstream HeaderStream;
		HeaderStream << "#pragma once" << std::endl;
		ExistingHeaders.insert(CurrentHeader);

		for (auto& CheckedElement : From.Elements)
		{
			if (CheckedElement.WrittenHeader)
				continue;
			if (CurrentHeader == CheckedElement.Header)
			{
				HeaderStream << CheckedElement.WriteCode(From);
				CheckedElement.WrittenHeader = true;
			}
		}

		bool ShouldWrite = true;
		if (std::filesystem::exists(CurrentHeader))
		{
			std::ifstream In = std::ifstream(CurrentHeader);
			std::stringstream SourceStream;
			SourceStream << In.rdbuf();
			In.close();
			ShouldWrite = HeaderStream.str() != SourceStream.str();
		}
		if (ShouldWrite)
		{
			std::ofstream OutFile = std::ofstream(CurrentHeader);
			OutFile << HeaderStream.rdbuf();
			OutFile.close();
		}
	}

	// Remove headers that don't exist anymore.
	for (auto& i : std::filesystem::directory_iterator(Dir))
	{
		if (!std::filesystem::is_regular_file(i))
			continue;
		bool Found = false;
		for (const std::string Header : ExistingHeaders)
		{
			// compare works like strcmp
			if (std::filesystem::path(Header).compare(i.path()) == 0)
			{
				Found = true;
			}
		}
		if (!Found)
		{
			std::filesystem::remove(i);
		}
	}
}