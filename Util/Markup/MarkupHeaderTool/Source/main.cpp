#include <Markup/MarkupStructure.h>
#include <Markup/MarkupVerify.h>
#include "Markup/MarkupParse.h"
#include "Markup/ParseError.h"
#include "Markup/WriteHeader.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
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

	std::vector<kui::MarkupParse::FileEntry> Entries;

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

	std::filesystem::create_directories(OutPath);

	try
	{
		for (const std::string& InPath : InPaths)
		{
			for (const auto& File : std::filesystem::directory_iterator(InPath))
			{
				kui::MarkupParse::FileEntry Entry;
				Entry.Name = File.path().filename().string();
				std::ifstream Source = std::ifstream(File.path());
				std::stringstream SourceStream;
				SourceStream << Source.rdbuf();
				Entry.Content = SourceStream.str();
				Entries.push_back(Entry);
			}
		}
	}
	catch (std::filesystem::filesystem_error& e)
	{
		std::cout << "fs error: " << e.what() << std::endl;
		return 2;
	}

	auto ParsedFiles = kui::MarkupParse::ParseFiles(Entries);

	if (kui::parseError::GetErrorCount())
	{
		std::cout << "Errors occurred - stopping." << std::endl;
		return 1;
	}

	kui::markupVerify::Verify(ParsedFiles);
	if (kui::parseError::GetErrorCount())
	{
		std::cout << "Errors occurred - stopping." << std::endl;
		return 1;
	}

	kui::writeHeader::WriteHeaders(OutPath, ParsedFiles);

	if (kui::parseError::GetErrorCount())
	{
		std::cout << "Errors occurred - stopping." << std::endl;
		return 1;
	}

}
