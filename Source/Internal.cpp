#include "Internal.h"
#include <SDL.h>
#include <GL/glew.h>
#include <KlemmUI/Rendering/Shader.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIBox.h>

void KlemmUI::Internal::InitSDL()
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
}

namespace KlemmUI::Internal
{
	Shader* WindowShader = nullptr;
}

void KlemmUI::Internal::InitGLContext(Window* From)
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* SDLWindow = static_cast<SDL_Window*>(From->GetSDLWindowPtr());

	SDL_GLContext OpenGLContext = SDL_GL_CreateContext(SDLWindow);

	SDL_GL_MakeCurrent(SDLWindow, OpenGLContext);

	GLenum GlewStatus = glewInit();

	if (GlewStatus != GLEW_OK)
	{
		Application::Error::Error((const char*)glewGetErrorString(GlewStatus), true);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	WindowShader = new Shader(Application::GetShaderPath() + "/postprocess.vert", Application::GetShaderPath() + "/postprocess.frag");
}

void KlemmUI::Internal::DrawWindow(Window* Target)
{
	WindowShader->Bind();
	glViewport(0, 0, (GLsizei)Target->GetSize().X, (GLsizei)Target->GetSize().Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, UIBox::GetUIFramebuffer());
	WindowShader->SetInt("u_ui", 0);
	WindowShader->SetInt("u_hasWindowBorder", !false);
	WindowShader->SetVec2("u_screenRes", Target->GetSize());
	WindowShader->SetVec3("u_borderColor", 1);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	WindowShader->Unbind();

	SDL_GL_SetSwapInterval(0);
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(Target->GetSDLWindowPtr()));
}