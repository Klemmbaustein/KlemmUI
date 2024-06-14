#pragma once
#include <string>
#include <cstdint>

namespace KlemmUI::Texture
{
	uint8_t* LoadTextureBytes(std::string File, size_t& Width, size_t& Height, bool Flipped = false);
	void FreeTextureBytes(uint8_t* Bytes);
	unsigned int LoadTexture(std::string File);
	unsigned int LoadTexture(uint8_t* Bytes, size_t Width, size_t Height);
	struct TextureInfo
	{
		unsigned int ID;
		size_t Width;
		size_t Height;
	};

	TextureInfo LoadTextureWithInfo(std::string File);

	void UnloadTexture(unsigned int ID);
}
