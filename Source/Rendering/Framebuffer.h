#pragma once
#include <KlemmUI/Vector2.h>

class Framebuffer
{
	unsigned int ID;
	unsigned int Textures[1];
public:
	Framebuffer(Vector2ui Resolution);
	void SetResolution(Vector2ui Resolution);
	~Framebuffer();
	void Bind();
	void Unbind();
	unsigned int GetID();
	unsigned int GetTexture(unsigned int Index);
};