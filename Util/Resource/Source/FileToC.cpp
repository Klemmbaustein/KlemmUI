#include "FileToC.h"
#include "Util.h"
#include <sstream>

std::string StringToCharArray(std::string ArrayName, std::string FromString)
{
	return BytesToCharArray(ArrayName, BinaryData{
		.Data = reinterpret_cast<uint8_t*>(FromString.data()),
		.Size = FromString.size()
		});
}

std::string BytesToCharArray(std::string ArrayName, BinaryData Data)
{
	std::stringstream OutString;
	OutString << "static char " << ArrayName << "[] = {\n\t";
	for (size_t i = 0; i < Data.Size; i++)
	{
		OutString << std::to_string(Data.Data[i]) << ", ";

		if (i % 16 == 15)
		{
			OutString << "\n\t";
		}
	}
	OutString << "\n};\n";
	return OutString.str();
}

std::string PathToName(std::string Path)
{
	ReplaceChar(Path, '/', "_slash_");
	ReplaceChar(Path, '.', "_dot_");
	ReplaceChar(Path, '-', "_dash_");
	return Path;
}

std::string WriteSourceFile(std::vector<std::pair<std::string, BinaryData>> Resources, std::string ProjectNames)
{
	std::stringstream OutString;

	OutString << "#include <stdint.h>\n";
	OutString << "#include <string.h>\n";

	OutString << "static const char* FileNames[] = {\n";
	for (const auto& i : Resources)
	{
		OutString << "\t\"" << i.first << "\",\n";
	}
	if (Resources.empty())
	{
		OutString << "\t\"\"\n";
	}
	OutString << "};\n";

	for (const auto& i : Resources)
	{
		OutString << BytesToCharArray(PathToName(i.first), i.second);
	}

	OutString << "size_t " << ProjectNames << "_GetResourceIndex(const char* FileName)\n{\n";
	OutString << "\tfor (size_t i = 0; i < " << Resources.size() << "; i++)\n\t{\n";
	OutString << "\t\tif (strcmp(FileNames[i], FileName) == 0)\n\t\t\treturn i;\n";
	OutString << "\t}\n\treturn SIZE_MAX;\n}\n";

	OutString << "size_t " << ProjectNames << "_GetResourceSize(size_t ResourceIndex)\n{\n";
	OutString << "\tswitch (ResourceIndex)\n\t{\n";
	for (size_t i = 0; i < Resources.size(); i++)
	{
		OutString << "\tcase " << i << ":\n";
		OutString << "\t\treturn " << Resources[i].second.Size << ";\n";
	}
	OutString << "\tdefault:\n\t\tbreak;\n";
	OutString << "\t}\n\treturn 0;\n}\n";

	OutString << "const char* const " << ProjectNames << "_GetResourceBytes(size_t ResourceIndex)\n{\n";
	OutString << "\tswitch (ResourceIndex)\n\t{\n";
	for (size_t i = 0; i < Resources.size(); i++)
	{
		OutString << "\tcase " << i << ":\n";
		OutString << "\t\treturn " << PathToName(Resources[i].first) << ";\n";
	}
	OutString << "\tdefault:\n\t\tbreak;\n";
	OutString << "\t}\n\treturn NULL;\n}\n";

	return OutString.str();
}
