#include <iostream>
#include <fstream>
#include <cstring>
#include <utility>
#include <optional>
#include "FileToC.h"
#include <filesystem>
#include "Util.h"

static void GetFilesInDirectory(std::filesystem::path Path,
	std::vector<std::filesystem::path>& OutPaths)
{
	for (const auto& i : std::filesystem::recursive_directory_iterator(Path))
	{
		OutPaths.push_back(std::filesystem::canonical(i.path()));
	}
}

int main(int argc, char** argv)
{
	std::string InPath;
	std::string OutPath;
	std::string ProjectName;

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
				std::cerr << "Error parsing command line: Unexpected '" << argv[i] << "'" << std::endl;
				return 1;
			}
			else if (strcmp(LastCommand, "-i") == 0)
			{
				InPath = argv[i];
			}
			else if (strcmp(LastCommand, "-o") == 0)
			{
				OutPath = argv[i];
			}
			else if (strcmp(LastCommand, "-n") == 0)
			{
				ProjectName = argv[i];
			}
		}
	}

	if (InPath.empty())
	{
		std::cerr << "No in path defined" << std::endl;
		return 1;
	}

	if (!std::filesystem::exists(InPath))
	{
		std::filesystem::create_directories(InPath);
	}

	if (OutPath.empty())
	{
		std::cerr << "No out path defined" << std::endl;
		return 1;
	}


	std::vector<std::filesystem::path> Paths;

	GetFilesInDirectory(InPath, Paths);

	std::ofstream out = std::ofstream(OutPath);
	std::filesystem::path CanonicalOutPath = std::filesystem::canonical(std::filesystem::path(OutPath));
	std::filesystem::current_path(InPath);

	std::vector<std::pair<std::string, BinaryData>> Files;

	std::vector<uint8_t*> FileBuffers;

	for (const std::filesystem::path& p : Paths)
	{
		std::string str = (const char*)std::filesystem::relative(p).u8string().c_str();
		ReplaceChar(str, '\\', "/");

		std::ifstream File = std::ifstream(p, std::ios::binary);

		File.seekg(0, std::ios::end);
		size_t Size = File.tellg();
		File.seekg(0, std::ios::beg);

		if (Size == 0)
		{
			std::cerr << "Warning: File " << str << " has a size of 0 - skipping" << std::endl;
			continue;
		}

		uint8_t* Buffer = new uint8_t[Size]();

		File.read((char*)Buffer, Size);

		Files.push_back(std::pair{ str, BinaryData{
			.Data = Buffer,
			.Size = Size
			} });
		FileBuffers.push_back(Buffer);
	}

	out << WriteSourceFile(Files, ProjectName);

	for (uint8_t* buf : FileBuffers)
	{
		delete[] buf;
	}
	FileBuffers.clear();

	return 0;
}
