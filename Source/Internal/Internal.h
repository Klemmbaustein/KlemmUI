#pragma once
#include <kui/Window.h>
#include <mutex>

namespace kui::internal
{
	void InitSDL();

	void InitGLContext(Window* From);

	void DrawWindow(Window* Target);
	
	void* GetGLFWMonitorOfWindow(Window* Target);

	std::u32string GetUnicodeString(std::string utf8, bool SameLength = false);

	extern std::mutex WindowCreationMutex;
}