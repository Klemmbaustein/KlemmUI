#pragma once
#include <KlemmUI/Window.h>

namespace KlemmUI::Internal
{
	typedef void* GLContext;

	void InitSDL();

	GLContext InitGLContext(Window* From);

	void DrawWindow(Window* Target);
}