#include <string>
#include <vector>
#include <utility>
#include <cstdint>

struct BinaryData
{
	uint8_t* Data;
	size_t Size;
};

std::string StringToCharArray(std::string ArrayName, std::string FromString);
std::string BytesToCharArray(std::string ArrayName, BinaryData Data);

std::string WriteSourceFile(std::vector<std::pair<std::string, BinaryData>> Resource, std::string ProjectNames);