#if __linux__
#include "SystemWM.h"
#include "SystemWM_X11.h"
#include <KlemmUI/Application.h>
#include <iostream>
#include <cstring>
#include <GL/gl.h>

std::string GetEnv(const std::string& var) {
	const char* val = std::getenv(var.c_str());
	if (val == nullptr) { // invalid to assign nullptr to std::string
		return "";
	}
	else {
		return val;
	}
}

void KlemmUI::SystemWM::X11Window::Create(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup)
{
	std::cout << "- [kui-X11]: $DISPLAY = '" << GetEnv("DISPLAY") << "'" << std::endl;

	XDisplay = XOpenDisplay(NULL);

	if (!XDisplay)
	{
		Application::Error::Error("Failed to open display", true);
		return;
	}

	::Window root = DefaultRootWindow(XDisplay);
	if (None == root)
	{
		Application::Error::Error("No root window found", true);
		XCloseDisplay(XDisplay);
		return;
	}

	GLint glxAttribs[] =
	{
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_RED_SIZE,       8,
		GLX_GREEN_SIZE,     8,
		GLX_BLUE_SIZE,      8,
		GLX_SAMPLE_BUFFERS, 0,
		GLX_SAMPLES,        0,
		None
	};

	int ScreenID = DefaultScreen(XDisplay);

	XVisualInfo* visual = glXChooseVisual(XDisplay, ScreenID, glxAttribs);

	XSetWindowAttributes attrib;
	memset(&attrib, sizeof(attrib), 0);
	attrib.border_pixel = BlackPixel(XDisplay, ScreenID);
	attrib.background_pixel = WhitePixel(XDisplay, ScreenID);
	attrib.override_redirect = True;
	attrib.colormap = XCreateColormap(XDisplay, RootWindow(XDisplay, ScreenID), visual->visual, AllocNone);
	attrib.event_mask = ExposureMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask;
	XWindow = XCreateWindow(XDisplay, root, Pos.X, Pos.Y, Size.X, Size.Y, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrib);

	if (None == XWindow)
	{
		Application::Error::Error("Failed to create window", true);
		XCloseDisplay(XDisplay);
		return;
	}
	XMapWindow(XDisplay, XWindow);

	if (visual == 0)
	{
		Application::Error::Error("Failed to create x11 visual", true);
		XCloseDisplay(XDisplay);
		return;
	}

	GLContext = glXCreateContext(XDisplay, visual, NULL, GL_TRUE);

	MakeContextCurrent();
}

void KlemmUI::SystemWM::X11Window::MakeContextCurrent() const
{
	glXMakeCurrent(XDisplay, XWindow, GLContext);
}

void KlemmUI::SystemWM::X11Window::UpdateWindow()
{
	while (XPending(XDisplay))
	{
		XEvent ev;
		XNextEvent(XDisplay, &ev);

		switch (ev.type)
		{
		case MotionNotify:
			CursorPosition = Vector2ui(ev.xmotion.x, ev.xmotion.y);
			break;
		case Expose:
		{
			Vector2ui NewSize = Vector2ui(ev.xexpose.width, ev.xexpose.height);
			if (WindowSize != NewSize)
			{
				WindowSize = NewSize;
				Parent->OnResized();
			}
			break;
		}
		case DestroyNotify:
			Parent->Close();
			break;
		case KeyPress:
			std::cout << "kp: " << ev.xkey.keycode << std::endl;
			break;
		case ButtonPress:
			std::cout << "bp: " << ev.xbutton.button << std::endl;
			break;
		default:
			std::cout << "unhandled event: " << ev.type << std::endl;
			break;
		}
	}
}

void KlemmUI::SystemWM::X11Window::Swap() const
{
	glXSwapBuffers(XDisplay, XWindow);
}

#endif
