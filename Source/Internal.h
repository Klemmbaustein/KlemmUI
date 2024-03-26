#pragma once
#include <KlemmUI/Window.h>

namespace KlemmUI::Internal
{
	void InitSDL();

	void InitGLContext(Window* From);

	void DrawWindow(Window* Target);
}