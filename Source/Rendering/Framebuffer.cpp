#include "Framebuffer.h"
#include <GL/glew.h>
#include <iostream>

Framebuffer::Framebuffer(Vec2ui Resolution)
{
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	// generate texture
	glGenTextures(1, &Textures[0]);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)Resolution.X, (GLint)Resolution.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Textures[0], 0);
	Unbind();
}

void Framebuffer::SetResolution(Vec2ui Resolution)
{
	glDeleteTextures(1, &Textures[0]);
	Bind();
	// generate texture
	glGenTextures(1, &Textures[0]);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)Resolution.X, (GLsizei)Resolution.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Textures[0], 0);
	Unbind();
}

Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, Textures);
	glDeleteFramebuffers(1, &ID);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Framebuffer::GetID() const
{
	return ID;
}

unsigned int Framebuffer::GetTexture(unsigned int Index)
{
	return Textures[Index];
}
