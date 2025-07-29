#include <kui/Rendering/OpenGLBackend.h>
#include <kui/App.h>
#include <kui/Window.h>
#include "OpenGL.h"
#include <kui/UI/UIScrollBox.h>
#include "VertexBuffer.h"
#include <algorithm>

using namespace kui;

kui::render::OpenGLBackend::OpenGLBackend(Window* From)
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

	From->Shaders.LoadShader("res:shaders/postprocess.vert", "res:shaders/postprocess.frag", "WindowShader");
	BackgroundShader = From->Shaders.LoadShader("res:shaders/uishader.vert", "res:shaders/uishader.frag", "UI Shader");

	UITextures[0] = 0;
	UITextures[1] = 0;

	BoxVertexBuffer = new GLVertexBuffer(
		{
			GLVertex(Vec2f(0, 0), 0),
			GLVertex(Vec2f(0, 1), 1),
			GLVertex(Vec2f(1, 0), 2),
			GLVertex(Vec2f(1, 1), 3)
		},
		{
			0u, 1u, 2u,
			1u, 2u, 3u
		});
}

kui::render::OpenGLBackend::~OpenGLBackend()
{
	GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;
	glDeleteFramebuffers(1, &UIBuffer);
	glDeleteTextures(NumBuffers, UITextures);
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

void kui::render::OpenGLBackend::DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color)
{
	BackgroundShader->SetVec3("u_color", Vec3f(1));
	BackgroundShader->SetInt("u_drawCorner", 0);
	BackgroundShader->SetInt("u_drawBorder", 0);
	BackgroundShader->SetFloat("u_opacity", 1);
	BackgroundShader->SetFloat("u_useTexture", 0);
	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));
	glUniform4f(glGetUniformLocation(BackgroundShader->GetShaderID(), "u_transform"),
		Position.X, Position.Y, Size.X, Size.Y
	);
	BoxVertexBuffer->Draw();
}

void kui::render::OpenGLBackend::BeginArea(Window* Target, RedrawBox Box)
{
	Vec2ui WindowSize = Target->GetSize();
	Box.Max += Vec2f(5) / Vec2f(WindowSize);
	Box.Min = Box.Min - Vec2f(5) / Vec2f(WindowSize);

	Box.Min = Box.Min.Clamp(-1, 1);
	Box.Max = Box.Max.Clamp(-1, 1);

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

	Shader* WindowShader = Target->Shaders.GetShader("WindowShader");
	glBindFramebuffer(GL_FRAMEBUFFER, TargetBuffer);
	WindowShader->Bind();
	glViewport(0, 0, (GLsizei)Target->GetSize().X, (GLsizei)Target->GetSize().Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UITextures[0]);
	WindowShader->SetInt("u_ui", 0);
	WindowShader->SetInt("u_hasWindowBorder",
		int((Target->GetWindowFlags() & Window::WindowFlag::Borderless) == Window::WindowFlag::Borderless
			&& !Target->GetWindowFullScreen()));
	WindowShader->SetVec2("u_screenRes", Target->GetSize());
	WindowShader->SetVec3("u_borderColor", Target->BorderColor);
	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	WindowShader->Unbind();
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
