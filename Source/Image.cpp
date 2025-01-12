#include <kui/Image.h>
#include "Internal/OpenGL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Util/stb_image.hpp"
#include <kui/Resource.h>
#include <iostream>
using namespace kui;

uint8_t* image::LoadImageBytes(std::string File, size_t& Width, size_t& Height, bool Flipped)
{
	int TextureWidth = 0;
	int TextureHeight = 0;
	int BitsPerPixel = 0;
	stbi_set_flip_vertically_on_load(!Flipped);
	
	resource::BinaryData TextureBytes = resource::GetBinaryFile(File);
	auto TextureBuffer = stbi_load_from_memory(TextureBytes.Data, int(TextureBytes.FileSize), &TextureWidth, &TextureHeight, &BitsPerPixel, 4);

	resource::FreeBinaryFile(TextureBytes);

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

void image::FreeImageBytes(uint8_t* Bytes)
{
	free(Bytes);
}

unsigned int image::LoadImage(std::string File)
{
	return LoadImageWithInfo(File).ID;
}

unsigned int kui::image::LoadImage(uint8_t* Bytes, size_t Width, size_t Height)
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

image::ImageInfo image::LoadImageWithInfo(std::string File)
{
	ImageInfo Ret = {};
	uint8_t* Bytes = LoadImageBytes(File, Ret.Width, Ret.Height);
	Ret.ID = LoadImage(Bytes, Ret.Width, Ret.Height);
	FreeImageBytes(Bytes);
	return Ret;
}

void image::UnloadImage(unsigned int ID)
{
	glDeleteTextures(1, &ID);
}
