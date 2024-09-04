#pragma once
#include <KlemmUI/Window.h>
#include <mutex>

namespace KlemmUI::Internal
{
	void InitSDL();

	void InitGLContext(Window* From);

	void DrawWindow(Window* Target);
	
	void* GetGLFWMonitorOfWindow(Window* Target);

	std::u32string GetUnicodeString(std::string utf8);

	extern std::mutex WindowCreationMutex;
}