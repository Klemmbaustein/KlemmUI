#include <KlemmUI/Rendering/Texture.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../Util/stb_image.hpp"
#include <iostream>

unsigned int Texture::LoadTexture(std::string File)
{
	return LoadTextureWithInfo(File).ID;
}

Texture::TextureInfo Texture::LoadTextureWithInfo(std::string File)
{
	int TextureWidth = 0;
	int TextureHeight = 0;
	int BitsPerPixel = 0;
	stbi_set_flip_vertically_on_load(true);
	auto TextureBuffer = stbi_load(File.c_str(), &TextureWidth, &TextureHeight, &BitsPerPixel, 4);

	if (TextureWidth == 0)
	{
		std::cerr << "stb image loading error: " << stbi_failure_reason() << ": failure image: " << File << std::endl;
	}
	TextureInfo Ret;
	Ret.Width = TextureWidth;
	Ret.Height = TextureHeight;
	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureWidth, TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);

	stbi_image_free(TextureBuffer);
	Ret.ID = TextureID;
	return Ret;
}

void Texture::UnloadTexture(unsigned int ID)
{
	glDeleteTextures(1, &ID);
}
