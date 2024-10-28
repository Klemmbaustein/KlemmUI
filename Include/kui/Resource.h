#include <cstdint>
#include <string>

namespace kui::resource
{
	struct BinaryData
	{
		const uint8_t* const Data = nullptr;
		const size_t FileSize = 0;
		const size_t ResourceType = SIZE_MAX;
	};

	BinaryData GetBinaryResource(const std::string& Path);
	std::string GetStringResource(const std::string& Path);

	bool ResourceExists(const std::string& Path);

	std::string GetStringFile(const std::string& Path);
	BinaryData GetBinaryFile(const std::string& Path);

	void FreeBinaryFile(BinaryData Data);

	bool FileExists(const std::string& Path);
}