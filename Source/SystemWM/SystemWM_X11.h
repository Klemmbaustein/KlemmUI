#if __linux__
#pragma once
#include "SystemWM.h"
#include "X11/Xlib.h"
#include <GL/glx.h>

namespace kui::systemWM
{
	class X11Window
	{
	public:
		X11Window() {};
		~X11Window() {};

		// X11's window class is just called "Window", yay!
		::kui::Window* Parent;

		void Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup);
		void Destroy();
		void SetTitle(std::string NewTitle) const;

		void MakeContextCurrent() const;

		void UpdateWindow();

		void Swap() const;
		thread_local static Display* XDisplay;
		thread_local static ::Window XRootWindow;
		::Window XWindow;
		GLXContext GLContext;
		XIC Input;
		XIM InputMethod;

		static bool IsLMBDown();
		static bool IsRMBDown();
		void SetCursor(Window::Cursor NewCursor) const;

		void SetMinSize(Vec2ui NewSize) const;
		void SetMaxSize(Vec2ui NewSize) const;
		
		void Maximize() const;
		void Minimize() const;
		void Restore() const;

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

		Vec2i CursorPosition;
		Vec2ui WindowSize;
		std::string TextInput;

		bool HasFocus = false;
	private:
		void HandleEvent(XEvent ev);
		void HandleKeyPress(KeySym Symbol, bool NewValue);
		static int QueryPointer(Vec2ui* MousePos);
	};
}

#endif