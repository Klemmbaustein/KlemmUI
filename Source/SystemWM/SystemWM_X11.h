#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM && !KLEMMUI_WITH_SDL
#pragma once
#include "SystemWM.h"
#include "X11/Xlib.h"
#include <GL/glx.h>
#include <cstdint>

namespace kui::systemWM
{
	class X11Window
	{
	public:
		X11Window() {};
		~X11Window() {};

		// X11's window class is just called "Window", yay!
		::kui::Window* Parent;

		void Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool AlwaysOnTop);
		void Destroy();
		void SetTitle(std::string NewTitle) const;

		void MakeContextCurrent() const;

		void UpdateWindow();

		void Swap() const;
		thread_local static Display* XDisplay;
		thread_local static ::Window XRootWindow;
		thread_local static uint32_t OpenedWindows;
		::Window XWindow;
		GLXContext GLContext;
		XIC Input;
		XIM InputMethod;

		static bool IsLMBDown();
		static bool IsRMBDown();
		void SetCursor(Window::Cursor NewCursor);

		void SetMinSize(Vec2ui NewSize);
		void SetMaxSize(Vec2ui NewSize);
		
		void Maximize() const;
		void Minimize() const;
		void Restore() const;

		void SetIcon(uint8_t* TextureBytes, size_t Width, size_t Height) const;

		float GetDPIScale();

		bool IsMaximized() const;
		bool IsMinimized();

		Vec2ui GetPosition() const;
		Vec2ui GetSize() const;

		void SetPosition(Vec2ui NewPosition) const;
		void SetSize(Vec2ui NewSize) const;
		
		static std::string GetClipboard();

		static Vec2ui GetMainScreenResolution();
		uint32_t GetMonitorRefreshRate() const;

		void SetBorderless(bool NewBorderless);
		void SetResizable(bool NewResizable);
		void SetAlwaysOnTop(bool NewAlwaysOnTop) const;

		Vec2i CursorPosition;
		Vec2ui WindowSize;
		std::string TextInput;

		bool HasFocus = false;
		bool Resizable = true;
		bool Borderless = false;
		bool HoveringCorner = false;
		int LastResizeCorner = false;

		Vec2ui MinSize;
		Vec2ui MaxSize;

		Window::Cursor CurrentCursor = Window::Cursor::Default;
	private:
		void HandleEvent(XEvent ev);
		void HandleKeyPress(KeySym Symbol, bool NewValue);
		static int QueryPointer(Vec2ui* MousePos);
	};
}

#endif