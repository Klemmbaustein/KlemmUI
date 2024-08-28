#pragma once
#include <string>
#include <cstdint>

namespace KlemmUI::Image
{
	uint8_t* LoadImageBytes(std::string File, size_t& Width, size_t& Height, bool Flipped = false);
	void FreeImageBytes(uint8_t* Bytes);
	unsigned int LoadImage(std::string File);
	unsigned int LoadImage(uint8_t* Bytes, size_t Width, size_t Height);
	struct ImageInfo
	{
		unsigned int ID;
		size_t Width;
		size_t Height;
	};

	ImageInfo LoadImageWithInfo(std::string File);

	void UnloadImage(unsigned int ID);
}
