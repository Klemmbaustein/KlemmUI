#include <KlemmUI/Rendering/Texture.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../Util/stb_image.hpp"
#include <iostream>
using namespace KlemmUI;

uint8_t* Texture::LoadTextureBytes(std::string File, size_t& Width, size_t& Height, bool Flipped)
{
	int TextureWidth = 0;
	int TextureHeight = 0;
	int BitsPerPixel = 0;
	stbi_set_flip_vertically_on_load(!Flipped);
	auto TextureBuffer = stbi_load(File.c_str(), &TextureWidth, &TextureHeight, &BitsPerPixel, 4);

	if (TextureWidth == 0)
	{
		std::cerr << "stb image loading error: " << stbi_failure_reason() << ": failure image: " << File << std::endl;
		Width = 0;
		Height = 0;
		return nullptr;
	}

	Width = TextureWidth;
	Height = TextureHeight;

	return TextureBuffer;
}

void Texture::FreeTextureBytes(uint8_t* Bytes)
{
	free(Bytes);
}

unsigned int Texture::LoadTexture(std::string File)
{
	return LoadTextureWithInfo(File).ID;
}

unsigned int KlemmUI::Texture::LoadTexture(uint8_t* Bytes, size_t Width, size_t Height)
{
	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// sizeof(GLsizei) != sizeof(size_t)
	// That's annoying...
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Bytes);

	return TextureID;
}

Texture::TextureInfo Texture::LoadTextureWithInfo(std::string File)
{
	TextureInfo Ret = {};
	uint8_t* Bytes = LoadTextureBytes(File, Ret.Width, Ret.Height);
	Ret.ID = LoadTexture(Bytes, Ret.Width, Ret.Height);
	FreeTextureBytes(Bytes);
	return Ret;
}

void Texture::UnloadTexture(unsigned int ID)
{
	glDeleteTextures(1, &ID);
}
