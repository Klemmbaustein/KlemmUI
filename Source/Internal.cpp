#include "Internal.h"
#include <SDL.h>
#include <GL/glew.h>
#include <KlemmUI/Rendering/Shader.h>
#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIBox.h>

#if _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <mutex>

void KlemmUI::Internal::InitSDL()
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
}

bool IsGLEWStarted = false;

KlemmUI::Internal::GLContext KlemmUI::Internal::InitGLContext(Window* From)
{
#if _WIN32
	SetProcessDPIAware();
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* SDLWindow = static_cast<SDL_Window*>(From->GetSDLWindowPtr());

	SDL_GLContext OpenGLContext = SDL_GL_CreateContext(SDLWindow);

	SDL_GL_MakeCurrent(SDLWindow, OpenGLContext);

	if (!IsGLEWStarted)
	{
		GLenum GLEWStatus = glewInit();
		if (GLEWStatus != GLEW_OK)
		{
			Application::Error::Error((const char*)glewGetErrorString(GLEWStatus), true);
		}
		IsGLEWStarted = true;
	}


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	std::string Name = SDL_GetWindowTitle(SDLWindow);
	From->Shaders.LoadShader("postprocess.vert", "postprocess.frag", "WindowShader");

	return (GLContext)OpenGLContext;
}

void KlemmUI::Internal::DrawWindow(Window* Target)
{
	Shader* WindowShader = Target->Shaders.GetShader("WindowShader");

	WindowShader->Bind();
	glViewport(0, 0, (GLsizei)Target->GetSize().X, (GLsizei)Target->GetSize().Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Target->UI.GetUIFramebuffer());
	WindowShader->SetInt("u_ui", 0);
	WindowShader->SetInt("u_hasWindowBorder", int((Target->GetWindowFlags() & Window::WindowFlag::Borderless) == Window::WindowFlag::Borderless));
	WindowShader->SetVec2("u_screenRes", Target->GetSize());
	WindowShader->SetVec3("u_borderColor", Target->BorderColor);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	WindowShader->Unbind();

	SDL_GL_SetSwapInterval(0);
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(Target->GetSDLWindowPtr()));
}
