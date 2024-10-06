#if _WIN32 && !KLEMMUI_CUSTOM_SYSTEMWM
#pragma once
#include <kui/Window.h>
#define NOMINMAX
#include <Windows.h>

namespace kui::systemWM
{
	class SysWindow
	{
	public:
		HWND WindowHandle = nullptr;
		HGLRC GLContext = nullptr;
		HDC DeviceContext = nullptr;
		Window* Parent = nullptr;
		HICON LastIcon = nullptr;

		bool Borderless = false;
		bool Resizable = false;
		bool Popup = false;
		int ActiveCursor = -1;

		Vec2ui Size;
		Vec2ui MinSize = 1;
		Vec2ui MaxSize = INT32_MAX;

		void SetSize(Vec2ui NewSize) const;

		std::string TextInput;

		void MakeContextActive() const;
	};
}
#endif