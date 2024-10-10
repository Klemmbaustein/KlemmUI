#pragma once
#include <kui/Vec2.h>

namespace kui::internal
{
	class Framebuffer
	{
		unsigned int ID;
		unsigned int Textures[1];
	public:
		Framebuffer(Vec2ui Resolution);
		void SetResolution(Vec2ui Resolution);
		~Framebuffer();
		void Bind();
		void Unbind();
		unsigned int GetID() const;
		unsigned int GetTexture(unsigned int Index);
	};
}