#pragma once
#include <string>
#include <cstdint>

namespace kui::resource
{
	struct BinaryData
	{
		const uint8_t* const Data = nullptr;
		const size_t FileSize = 0;
		const size_t ResourceType = SIZE_MAX;
	};

	class UIResourceProvider
	{
	public:
		virtual ~UIResourceProvider() = default;

		virtual bool FileExists(const std::string& Path) = 0;
		virtual BinaryData GetFile(const std::string& File) = 0;
	};
}