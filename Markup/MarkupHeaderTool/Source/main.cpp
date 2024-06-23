#include <Markup/MarkupStructure.h>
#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <filesystem>

using namespace KlemmUI::MarkupStructure;

int main(int argc, const char** argv)
{
	std::vector<std::string> InPaths;
	std::string OutPath;

	const char* LastCommand = nullptr;
	for (int i = 1; i < argc; i++)
	{
		if (strlen(argv[i]) > 0 && argv[i][0] == '-')
		{
			LastCommand = argv[i];
		}
		else
		{
			if (LastCommand == nullptr)
			{
				std::cout << "Error parsing command line: Unexpected '" << argv[i] << "'" << std::endl;
				return 1;
			}
			else if (strcmp(LastCommand, "-i") == 0)
			{
				InPaths.push_back(argv[i]);
			}
			else if (strcmp(LastCommand, "-o") == 0)
			{
				OutPath = argv[i];
			}
		}
	}

	std::vector<KlemmUI::MarkupParse::FileEntry> Entries;
	try
	{
		for (const std::string& InPath : InPaths)
		{
			for (const auto& File : std::filesystem::directory_iterator(InPath))
			{
				KlemmUI::MarkupParse::FileEntry Entry;
				Entry.Name = File.path().filename().string();
				std::ifstream Source = std::ifstream(File.path());
				Entry.Content = std::string(std::istreambuf_iterator<char>(Source.rdbuf()),
					std::istreambuf_iterator<char>());
				Entries.push_back(Entry);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 2;
	}

	auto ParsedFiles = KlemmUI::MarkupParse::ParseFiles(Entries);

	if (KlemmUI::ParseError::GetErrorCount())
	{
		std::cout << "Errors occurred - stopping." << std::endl;
		return 1;
	}

	for (auto& i : ParsedFiles.Elements)
	{
		i.WriteHeader(OutPath, ParsedFiles);
	}
	if (KlemmUI::ParseError::GetErrorCount())
	{
		std::cout << "Errors occurred - stopping." << std::endl;
		return 1;
	}

}
