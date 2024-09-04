#if __linux__
#include "SystemWM.h"
#include "X11/Xlib.h"
#include <GL/glx.h>

namespace KlemmUI::SystemWM
{
	class X11Window
	{
	public:
		// X11's window class is just called "Window", yay!
		::KlemmUI::Window* Parent;

		void Create(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup);
		void Destroy();

		void MakeContextCurrent() const;

		void UpdateWindow();

		void Swap() const;
		static Display* XDisplay;
		static ::Window XRootWindow;
		::Window XWindow;
		GLXContext GLContext;

		static bool IsLMBDown();

		Vector2ui CursorPosition;
		Vector2ui WindowSize;

		bool HasFocus = false;
	private:
		static int QueryPointer(Vector2ui* MousePos);
	};
}

#endif