#include <kui/UI/UIBlurBackground.h>
#include <kui/Rendering/Shader.h>
#include <kui/Window.h>
#include <kui/Resource.h>
#include "../Rendering/VertexBuffer.h"
#include <GL/glew.h>
#include <iostream>

const float BlurScale = 0.01f;

Vec2ui kui::UIBlurBackground::GetPixelSize()
{
	return Vec2ui(
		ParentWindow->GetSize().X * Size.X * 0.5f * BlurScale,
		ParentWindow->GetSize().Y * Size.X * 0.5f * BlurScale
	);
}

std::set<kui::UIBlurBackground*> kui::UIBlurBackground::BlurBackgrounds;

void kui::UIBlurBackground::CreateBlurBuffer()
{
	if (BackgroundBuffer)
	{
		glDeleteBuffers(1, &BackgroundBuffer);
		glDeleteTextures(1, &BackgroundTexture);
	}

	const Vec2ui Size = GetPixelSize();
	glGenFramebuffers(1, &BackgroundBuffer);
	// create floating point color buffer
	glGenTextures(1, &BackgroundTexture);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB8,
		Size.X,
		Size.Y,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, BackgroundBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BackgroundTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

kui::UIBlurBackground::UIBlurBackground(bool Horizontal, Vec2f Position, Vec3f Color, float Opacity, Vec2f MinScale)
	: UIBackground(Horizontal, Position, Color, MinScale, Window::GetActiveWindow()->Shaders.LoadShader(
		"res:shaders/uishader.vert",
		"res:shaders/blursurface.frag",
		"blur background shader"))
{
	this->Opacity = Opacity;
	BlurShader = Window::GetActiveWindow()->Shaders.LoadShader(
		"res:shaders/uiblur.vert",
		"res:shaders/uiblur.frag",
		"UI blurring shader"
	);
	BlurBackgrounds.insert(this);
}

kui::UIBlurBackground::~UIBlurBackground()
{
	if (BackgroundBuffer)
	{
		glDeleteBuffers(1, &BackgroundBuffer);
		glDeleteTextures(1, &BackgroundTexture);
	}
	BlurBackgrounds.erase(this);
}

void kui::UIBlurBackground::Draw()
{
	if (!BoxVertexBuffer)
	{
		return;
	}
	Vec2ui WindowSize = ParentWindow->GetSize();
	const Vec2ui PixelSize = GetPixelSize();

	if (OldSize != PixelSize)
	{
		OldSize = PixelSize;
		CreateBlurBuffer();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, BackgroundBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ParentWindow->UI.UITexture);

	glViewport(0, 0, PixelSize.X, PixelSize.Y);
	glDisable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	BlurShader->Bind();
	BlurShader->SetVec2("u_scale", Size * 2);
	BlurShader->SetVec2("u_position", (OffsetPosition + 1) / 2);
	BlurShader->SetVec2("u_pixelSize", Vec2f(0.1f) / Vec2f(PixelSize));
	BoxVertexBuffer->Draw();
	BlurShader->Unbind();
	glViewport(0, 0, WindowSize.X, WindowSize.Y);
	glBindFramebuffer(GL_FRAMEBUFFER, ParentWindow->UI.UIBuffer);
	glEnable(GL_SCISSOR_TEST);

	BackgroundShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BackgroundTexture);
	BoxVertexBuffer->Bind();
	ScrollTick(BackgroundShader);
	BackgroundShader->SetVec3("u_color", Color);
	BackgroundShader->SetVec3("u_borderColor", BorderColor);

	WindowSize = WindowSize / 2;

	const Vec2f Pos = Vec2f(Vec2i(OffsetPosition * WindowSize)) / WindowSize;
	const Vec2f Res = Vec2f(Vec2i(Size * WindowSize)) / WindowSize;

	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"), Pos.X, Pos.Y, Res.X, Res.Y);
	BackgroundShader->SetFloat("u_opacity", Opacity);
	BackgroundShader->SetInt("u_drawBorder", BorderRadius != 0);
	BackgroundShader->SetInt("u_drawCorner", CornerRadius != 0);
	BackgroundShader->SetFloat("u_borderScale", GetBorderSize(BorderRadius, BorderSizeMode));
	BackgroundShader->SetFloat("u_cornerScale", GetBorderSize(CornerRadius, CornerSizeMode));
	BackgroundShader->SetInt("u_cornerFlags", int(CornerFlags));
	BackgroundShader->SetInt("u_borderFlags", int(BorderFlags));
	BackgroundShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());
	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));

	BoxVertexBuffer->Draw();
	DrawBackground();
	BoxVertexBuffer->Unbind();
	BackgroundShader->Unbind();

}
