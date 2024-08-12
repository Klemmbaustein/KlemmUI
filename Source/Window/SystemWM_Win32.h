#if _WIN32
#pragma once
#include <KlemmUI/Window.h>
#define NOMINMAX
#include <Windows.h>

namespace KlemmUI::SystemWM
{
	class SysWindow
	{
	public:
		HWND WindowHandle = nullptr;
		HGLRC GLContext = nullptr;
		HDC DeviceContext = nullptr;
		Window* Parent = nullptr;

		bool Borderless = false;
		bool Resizable = false;
		bool Popup = false;

		Vector2ui Size;

		std::string TextInput;

		void MakeContextActive() const;
	};
}
#endif