#include <kui/Rendering/OpenGLBackend.h>
#include <kui/App.h>
#include <kui/Window.h>
#include "OpenGL.h"
#include <kui/UI/UIScrollBox.h>
#include "VertexBuffer.h"
#include <algorithm>

using namespace kui;
bool render::OpenGLBackend::UseAlphaBuffer = false;

kui::render::OpenGLBackend::OpenGLBackend()
{
	UITextures[0] = 0;
	UITextures[1] = 0;
}

kui::render::OpenGLBackend::~OpenGLBackend()
{
	GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;
	glDeleteFramebuffers(1, &UIBuffer);
	glDeleteTextures(NumBuffers, UITextures);
}

void kui::render::OpenGLBackend::Initialize(Window* From)
{
	static bool IsGLEWStarted = false;
	From->MakeContextCurrent();

#ifndef KLEMMUI_WEB_BUILD
	if (!IsGLEWStarted)
	{
#if _WIN32
		GLenum GLEWStatus = glewInit();
#else
		GLenum GLEWStatus = glewContextInit();
#endif
		if (GLEWStatus != GLEW_OK)
		{
			app::error::Error((const char*)glewGetErrorString(GLEWStatus), true);
		}
		IsGLEWStarted = true;
	}
#else
#endif

	WindowShader = From->Shaders.LoadShader("res:shaders/postprocess.vert", "res:shaders/postprocess.frag", "Window Shader");
	BackgroundShader = From->Shaders.LoadShader("res:shaders/uishader.vert", "res:shaders/uishader.frag", "UI Shader");
	TextShader = From->Shaders.LoadShader("res:shaders/text.vert", "res:shaders/text.frag", "Font Shader");

	UITextures[0] = 0;
	UITextures[1] = 0;

	BoxVertexBuffer = new GLVertexBuffer({
		GLVertex(Vec2f(0, 0), 0),
		GLVertex(Vec2f(0, 1), 1),
		GLVertex(Vec2f(1, 0), 2),
		GLVertex(Vec2f(1, 1), 3) },
	{
		0u, 1u, 2u,
		1u, 2u, 3u
	});
}

void kui::render::OpenGLBackend::CreateBuffer(Vec2ui Size)
{
	if (UIBuffer)
	{
		glDeleteFramebuffers(1, &UIBuffer);
		GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;
		glDeleteTextures(NumBuffers, UITextures);
	}
	UIBuffer = 0;

	glGenFramebuffers(1, &UIBuffer);
	GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;

	glGenTextures(NumBuffers, UITextures);
	glBindTexture(GL_TEXTURE_2D, UITextures[0]);

	GLsizei x = (GLsizei)Size.X, y = (GLsizei)Size.Y;
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA16F,
		x,
		y,
		0,
		GL_RGBA,
		GL_FLOAT,
		nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITextures[0], 0);

	if (UseAlphaBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, UITextures[1]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			x,
			y,
			0,
			GL_RGBA,
			GL_FLOAT,
			nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, UITextures[1], 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

UIBackgroundState* kui::render::OpenGLBackend::MakeBackground()
{
	return new GLUIBackgroundState(BackgroundShader);
}

void kui::render::OpenGLBackend::BeginFrame(Window* Target)
{
	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glClearColor(0, 0, 0, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SCISSOR_TEST);
	if (UseAlphaBuffer)
	{
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	}
	Vec2ui WindowSize = Target->GetSize();

	glViewport(0, 0, (GLint)WindowSize.X, (GLint)WindowSize.Y);
}

void kui::render::OpenGLBackend::EndFrame(Window* Target)
{
	Vec2ui WindowSize = Target->GetSize();
	glDisable(GL_SCISSOR_TEST);
	glScissor(0, 0, (GLsizei)WindowSize.X, (GLsizei)WindowSize.Y);
}

void kui::render::OpenGLBackend::DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color, unsigned int Texture)
{
	BackgroundShader->Bind();
	BackgroundShader->SetVec3("u_color", Color);
	BackgroundShader->SetInt("u_drawCorner", 0);
	BackgroundShader->SetInt("u_drawBorder", 0);
	BackgroundShader->SetFloat("u_opacity", 1);
	BackgroundShader->SetInt("u_useTexture", Texture ? 1 : 0);

	if (Texture)
	{
		if (Texture != UINT32_MAX)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture);
		}
		BackgroundShader->SetInt("u_texture", 0);
	}
	BackgroundShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());

	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));
	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"),
		Position.X, Position.Y, Size.X, Size.Y
	);
	BoxVertexBuffer->Draw();
	BackgroundShader->Unbind();
}

void kui::render::OpenGLBackend::BeginArea(Window* Target, RedrawBox Box)
{
	Vec2ui WindowSize = Target->GetSize();

	Vec2f Pos = (Box.Min / 2 + 0.5f) * Vec2f(WindowSize);
	Vec2f Res = (Box.Max - Box.Min) / 2 * Vec2f(WindowSize);

	ScissorXY = Vec2ui(uint64_t(Pos.X), uint64_t(Pos.Y));
	ScissorWH = Vec2ui(
		std::clamp((GLsizei)Res.X + 1, 0, (GLsizei)WindowSize.X),
		std::clamp((GLsizei)Res.Y + 1, 0, (GLsizei)WindowSize.Y)
	);

	glScissor(
		GLint(ScissorXY.X),
		GLint(ScissorXY.Y),
		GLsizei(ScissorWH.X),
		GLsizei(ScissorWH.Y)
	);

	glClear(GL_COLOR_BUFFER_BIT);
}

void kui::render::OpenGLBackend::DrawToWindow(Window* Target)
{
	if (!CanDrawToWindow)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, TargetBuffer);
	WindowShader->Bind();
	glViewport(0, 0, (GLsizei)Target->GetSize().X, (GLsizei)Target->GetSize().Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UITextures[0]);
	WindowShader->SetInt("u_ui", 0);
	WindowShader->SetInt("u_flipImage", int(this->FlipImage));
	WindowShader->SetInt("u_hasWindowBorder",
		int((Target->GetWindowFlags() & Window::WindowFlag::Borderless) == Window::WindowFlag::Borderless
			&& !Target->GetWindowFullScreen()));
	WindowShader->SetVec2("u_screenRes", Target->GetSize());
	WindowShader->SetVec3("u_borderColor", Target->BorderColor);
	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	WindowShader->Unbind();
}

FontRenderData* kui::render::OpenGLBackend::MakeFont(uint8_t* ImageData, uint32_t Width, uint32_t Height, float CharSize)
{
	GLFontRenderData* NewData = new GLFontRenderData();
	glGenTextures(1, &NewData->FontTexture);
	glBindTexture(GL_TEXTURE_2D, NewData->FontTexture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_ALPHA,
		Width,
		Height,
		0,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		ImageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return NewData;
}

DrawableText* kui::render::OpenGLBackend::MakeText(FontRenderData* Data, const std::vector<Font::RenderGlyph>& Glyphs,
	float Scale, Vec3f Color, float Opacity)
{
	auto GLData = static_cast<GLFontRenderData*>(Data);

	GLuint NewVAO = 0, NewVBO = 0;
	glGenVertexArrays(1, &NewVAO);
	glBindVertexArray(NewVAO);
	glGenBuffers(1, &NewVBO);
	glBindBuffer(GL_ARRAY_BUFFER, NewVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * GLData->FontVertexBufferCapacity, 0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, texCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (const void*)offsetof(FontVertex, color));
	glEnableVertexAttribArray(0);

	if (GLData->FontVertexBufferCapacity < Glyphs.size())
	{
		GLData->FontVertexBufferCapacity = uint32_t(Glyphs.size());
		glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * GLData->FontVertexBufferCapacity, 0, GL_STATIC_DRAW);
		delete[] GLData->FontVertexBufferData;
		GLData->FontVertexBufferData = new FontVertex[GLData->FontVertexBufferCapacity * 6];
	}

	uint32_t NumVertices = 0;

	FontVertex* vData = GLData->FontVertexBufferData;

	for (auto& seg : Glyphs)
	{
		auto& StartPos = seg.Position;
		auto& g = *seg.Target;
		vData[0].position = StartPos + Vec2f(0, g.Size.Y); vData[0].texCoords = g.TexCoordStart + Vec2f(0, g.TexCoordOffset.Y);
		vData[1].position = StartPos + g.Size;             vData[1].texCoords = g.TexCoordStart + g.TexCoordOffset;
		vData[2].position = StartPos + Vec2f(g.Size.X, 0); vData[2].texCoords = g.TexCoordStart + Vec2f(g.TexCoordOffset.X, 0);
		vData[3].position = StartPos;                      vData[3].texCoords = g.TexCoordStart;
		vData[4].position = StartPos + Vec2f(0, g.Size.Y); vData[4].texCoords = g.TexCoordStart + Vec2f(0, g.TexCoordOffset.Y);
		vData[5].position = StartPos + Vec2f(g.Size.X, 0); vData[5].texCoords = g.TexCoordStart + Vec2f(g.TexCoordOffset.X, 0);
		vData[0].color = seg.Color;		vData[1].color = seg.Color;
		vData[2].color = seg.Color;		vData[3].color = seg.Color;
		vData[4].color = seg.Color;		vData[5].color = seg.Color;
		vData += 6;
		NumVertices += 6;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex) * NumVertices, GLData->FontVertexBufferData);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return new GLDrawableText(this, NewVAO, NewVBO, NumVertices, GLData->FontTexture, Scale, Color, Opacity);
}


void kui::render::OpenGLBackend::UpdateScroll(ScrollObject* Scroll, Shader* UsedShader, UIBackgroundState* Target)
{
	if (Scroll != nullptr)
		UsedShader->SetVec3("u_offset",
			Vec3f(-Scroll->GetOffset(), Scroll->GetPosition().Y, Scroll->GetScale().Y));
	else
		UsedShader->SetVec3("u_offset", Vec3f(0, -1000, 1000));
}

void kui::render::GLUIBackgroundState::Draw(render::RenderBackend* With, Vec2f Position, Vec2f Size, ScrollObject* Scroll)
{
	auto Backend = static_cast<OpenGLBackend*>(With);

	if (!Backend || !UsedShader)
	{
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	UsedShader->Bind();
	Backend->UpdateScroll(Scroll, UsedShader, this);
	UsedShader->SetVec3("u_color", Color);
	UsedShader->SetVec3("u_borderColor", BorderColor);

	UISize DrawnBorderRadius = BorderRadius;

	//if (this == ParentWindow->UI.KeyboardFocusBox)
	//{
	//	if (DrawnBorderRadius.Value == 0)
	//	{
	//		DrawnBorderRadius = 2_px;
	//	}
	//	else
	//	{
	//		DrawnBorderRadius.Value *= 2;
	//	}
	//}

	glUniform4f(UsedShader->GetUniformLocation("u_transform"), Position.X, Position.Y, Size.X, Size.Y);
	UsedShader->SetFloat("u_opacity", Opacity);
	UsedShader->SetInt("u_drawBorder", DrawnBorderRadius.Value != 0);
	UsedShader->SetInt("u_drawCorner", CornerRadius.Value != 0);
	UsedShader->SetFloat("u_borderScale", UIBackground::GetBorderSize(DrawnBorderRadius));
	UsedShader->SetFloat("u_cornerScale", UIBackground::GetBorderSize(CornerRadius));
	UsedShader->SetInt("u_cornerFlags", int(CornerFlags));
	UsedShader->SetInt("u_borderFlags", int(BorderFlags));
	UsedShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());
	UsedShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));

	UsedShader->SetInt("u_useTexture", (int)UseTexture);
	Backend->BoxVertexBuffer->Draw();
	UsedShader->Unbind();
}

kui::render::GLDrawableText::GLDrawableText(OpenGLBackend* Parent, unsigned int VAO, unsigned int VBO,
	unsigned int NumVerts, unsigned int Texture, float Scale, Vec3f Color, float Opacity)
	: DrawableText(Scale, Color, Opacity)
{
	this->Parent = Parent;
	this->VAO = VAO;
	this->VBO = VBO;
	this->NumVerts = NumVerts;
	this->Texture = Texture;
}

kui::render::GLDrawableText::~GLDrawableText()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

kui::render::GLFontRenderData::~GLFontRenderData()
{
	glDeleteTextures(1, &FontTexture);
	glDeleteBuffers(1, &FontVertexBufferId);
	if (FontVertexBufferData)
	{
		delete[] FontVertexBufferData;
	}
}

void kui::render::GLDrawableText::Draw(ScrollObject* CurrentScrollObject, Vec2f Pos)
{
	Pos.X = Pos.X * 450 * Window::GetActiveWindow()->GetAspectRatio();
	Pos.Y = Pos.Y * -450;

	Shader* TextShader = Parent->TextShader;
	glBindVertexArray(VAO);
	TextShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	TextShader->SetInt("u_texture", 0);
	TextShader->SetVec3("textColor", Vec3f(Color.X, Color.Y, Color.Z));
	TextShader->SetFloat("u_aspectratio", Window::GetActiveWindow()->GetAspectRatio());
	TextShader->SetVec3("transform", Vec3f(Pos.X, Pos.Y, Scale));
	TextShader->SetVec2("u_screenRes", Vec2f(Window::GetActiveWindow()->GetSize().Y * 1.5f));
	TextShader->SetFloat("u_opacity", Opacity);
	if (CurrentScrollObject != nullptr)
	{
		auto ScrollPos = CurrentScrollObject->GetPosition();

		TextShader->SetVec3("u_offset",
			Vec3f(-CurrentScrollObject->GetOffset(), ScrollPos.Y, CurrentScrollObject->GetScale().Y));
	}
	else
		TextShader->SetVec3("u_offset", Vec3f(0.0f, -1000.0f, 1000.0f));
	glDrawArrays(GL_TRIANGLES, 0, NumVerts);
	TextShader->Unbind();
	glBindVertexArray(0);
}

unsigned int kui::render::OpenGLBackend::CreateTexture(uint8_t* Bytes, std::size_t Width, std::size_t Height)
{
	GLuint TextureID = 0;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Width < 64 ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Width < 64 ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// sizeof(GLsizei) != sizeof(size_t)
	// That's annoying...
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	return TextureID;
}

void kui::render::OpenGLBackend::FreeTexture(unsigned int Target)
{
	glDeleteTextures(1, &Target);
}
