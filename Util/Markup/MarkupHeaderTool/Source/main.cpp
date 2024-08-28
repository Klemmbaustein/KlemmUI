#include <Markup/MarkupStructure.h>
#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <filesystem>

void PrintUsage()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "KlemmUIHT -i ./in/path1 -i ./in/path/2 -o ./out/path" << std::endl;
}

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

	if (InPaths.empty())
	{
		std::cout << "No in path defined" << std::endl;
		PrintUsage();
		return 3;
	}
	if (OutPath.empty())
	{
		std::cout << "No out path defined" << std::endl;
		PrintUsage();
		return 3;
	}

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
	catch (std::filesystem::filesystem_error& e)
	{
		std::cout << "fs error: " << e.what() << std::endl;
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
