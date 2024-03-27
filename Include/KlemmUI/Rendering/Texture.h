#pragma once
#include <string>

namespace KlemmUI::Texture
{
	unsigned int LoadTexture(std::string File);
	struct TextureInfo
	{
		unsigned int ID;
		size_t Width;
		size_t Height;
	};

	TextureInfo LoadTextureWithInfo(std::string File);

	void UnloadTexture(unsigned int ID);
}