#if KLEMMUI_WEB_BUILD
#pragma once
#include <kui/Window.h>
#include <GLFW/glfw3.h>

namespace kui::systemWM
{
	class SysWindow
	{
	public:
		Window* Parent = nullptr;
		GLFWwindow* GLWindow = nullptr;

		std::array<GLFWcursor*, size_t(Window::Cursor::End)> LoadedCursors;

		std::string TextInput;
	};
}
#endif