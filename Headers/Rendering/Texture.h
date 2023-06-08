#pragma once
#include <string>

namespace Texture
{
	unsigned int LoadTexture(std::string File);
	void UnloadTexture(unsigned int ID);
}