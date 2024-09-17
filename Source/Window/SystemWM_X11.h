#if __linux__
#include "SystemWM.h"
#include "X11/Xlib.h"
#include <GL/glx.h>

namespace KlemmUI::SystemWM
{
	class X11Window
	{
	public:
		X11Window() {};
		~X11Window() {};

		// X11's window class is just called "Window", yay!
		::KlemmUI::Window* Parent;

		void Create(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup);
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

		void SetMinSize(Vector2ui NewSize);
		void SetMaxSize(Vector2ui NewSize);
		
		void Maximize();
		void Minimize();
		void Restore();

		Vector2ui GetPosition();
		Vector2ui GetSize();

		void SetPosition(Vector2ui NewPosition);
		void SetSize(Vector2ui NewSize);
		
		static Vector2ui GetMainScreenResolution();
		uint32_t GetMonitorRefreshRate() const;

		Vector2i CursorPosition;
		Vector2ui WindowSize;
		std::string TextInput;

		bool HasFocus = false;
	private:
		void HandleKeyPress(KeySym Symbol, bool NewValue);
		static int QueryPointer(Vector2ui* MousePos);
	};
}

#endif