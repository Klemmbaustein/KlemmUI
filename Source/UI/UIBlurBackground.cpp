#include <kui/UI/UIBlurBackground.h>
#include <kui/Rendering/Shader.h>
#include <kui/Window.h>
#include <kui/Rendering/OpenGLBackend.h>
#include "../Rendering/VertexBuffer.h"
#include "../Rendering/OpenGL.h"

const float BlurScale = 0.2f;
const int BlurAmount = 15;

kui::Vec2ui kui::UIBlurBackground::GetPixelSize()
{
	return Vec2ui::Max(Vec2ui(
		uint32_t(float(ParentWindow->GetSize().X) * Size.X * 0.5f * BlurScale),
		uint32_t(float(ParentWindow->GetSize().Y) * Size.Y * 0.5f * BlurScale)
	), 1);
}

thread_local std::set<kui::UIBlurBackground*> kui::UIBlurBackground::BlurBackgrounds;

void kui::UIBlurBackground::CreateBlurBuffers()
{
	if (BuffersLoaded)
	{
		glDeleteFramebuffers(2, BackgroundBuffers);
		glDeleteTextures(2, BackgroundTextures);
	}

	const Vec2ui Size = GetPixelSize();

	glGenFramebuffers(2, BackgroundBuffers);
	glGenTextures(2, BackgroundTextures);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, BackgroundBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, BackgroundTextures[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, GLsizei(Size.X), GLsizei(Size.Y), 0, GL_RGBA, GL_FLOAT, nullptr
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BackgroundTextures[i], 0
		);
	}
	BuffersLoaded = true;
}

kui::UIBlurBackground::UIBlurBackground(bool Horizontal, Vec2f Position, Vec3f Color, float Opacity, SizeVec MinScale)
	: UIBackground(Horizontal, Position, Color, MinScale)
{
	BackgroundBuffers[0] = 0;
	BackgroundBuffers[1] = 0;
	BackgroundTextures[0] = 0;
	BackgroundTextures[1] = 0;
	this->State->Opacity = Opacity;
	BlurBackgrounds.insert(this);
}

kui::UIBlurBackground::~UIBlurBackground()
{
	if (BuffersLoaded)
	{
		glDeleteFramebuffers(2, BackgroundBuffers);
		glDeleteTextures(2, BackgroundTextures);
	}
	BlurBackgrounds.erase(this);
}

void kui::UIBlurBackground::Draw(render::RenderBackend* Backend)
{
	auto GL = dynamic_cast<render::OpenGLBackend*>(Backend);

	if (!GL)
	{
		UIBackground::Draw(Backend);
		return;
	}

	if (!this->BackgroundShader)
	{
		this->BackgroundShader = Window::GetActiveWindow()->Shaders.LoadShader(
			"res:shaders/uishader.vert",
			"res:shaders/blursurface.frag",
			"blur background shader");
		BlurShader = Window::GetActiveWindow()->Shaders.LoadShader(
			"res:shaders/uiblur.vert",
			"res:shaders/uiblur.frag",
			"UI blurring shader"
		);
	}

	Vec2ui WindowSize = ParentWindow->GetSize();
	const Vec2ui PixelSize = GetPixelSize();

	if (OldSize != PixelSize)
	{
		OldSize = PixelSize;
		CreateBlurBuffers();
	}
	glViewport(0, 0, (GLsizei)PixelSize.X, (GLsizei)PixelSize.Y);
	glDisable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	bool Horizontal = true, FirstIteration = true;
	BlurShader->Bind();
	BlurShader->SetVec2("u_scale", Size / 2);
	BlurShader->SetVec2("u_position", (OffsetPosition + 1) / 2);

	for (unsigned int i = 0; i < BlurAmount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, BackgroundBuffers[Horizontal]);
		BlurShader->SetInt("u_horizontal", Horizontal);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(
			GL_TEXTURE_2D, FirstIteration ? GL->UITextures[0] : BackgroundTextures[!Horizontal]
		);
		GL->BoxVertexBuffer->Draw();
		BlurShader->SetVec2("u_scale", 1);
		BlurShader->SetVec2("u_position", 0);
		Horizontal = !Horizontal;
		if (FirstIteration)
			FirstIteration = false;
	}
	BlurShader->Unbind();
	glViewport(0, 0, (GLsizei)WindowSize.X, (GLsizei)WindowSize.Y);
	glBindFramebuffer(GL_FRAMEBUFFER, GL->UIBuffer);
	glEnable(GL_SCISSOR_TEST);

	BackgroundShader->Bind();

	glBindTexture(GL_TEXTURE_2D, BackgroundTextures[0]);
	GL->UpdateScroll(CurrentScrollObject, BackgroundShader, State);
	BackgroundShader->SetVec3("u_color", State->Color);
	BackgroundShader->SetVec3("u_borderColor", State->BorderColor);

	glUniform4f(BackgroundShader->GetUniformLocation("u_transform"), OffsetPosition.X, OffsetPosition.Y, Size.X, Size.Y);
	BackgroundShader->SetFloat("u_opacity", State->Opacity);
	BackgroundShader->SetInt("u_drawBorder", State->BorderRadius.Value != 0);
	BackgroundShader->SetInt("u_drawCorner", State->CornerRadius.Value != 0);
	BackgroundShader->SetFloat("u_borderScale", GetBorderSize(State->BorderRadius));
	BackgroundShader->SetFloat("u_cornerScale", GetBorderSize(State->CornerRadius));
	BackgroundShader->SetInt("u_cornerFlags", int(State->CornerFlags));
	BackgroundShader->SetInt("u_borderFlags", int(State->BorderFlags));
	BackgroundShader->SetFloat("u_aspectRatio", Window::GetActiveWindow()->GetAspectRatio());
	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		(float)Window::GetActiveWindow()->GetSize().X,
		(float)Window::GetActiveWindow()->GetSize().Y));

	GL->BoxVertexBuffer->Draw();
	DrawBackground(Backend);
	BackgroundShader->Unbind();

}
