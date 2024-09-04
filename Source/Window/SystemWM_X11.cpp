#if __linux__
#include "SystemWM.h"
#include "SystemWM_X11.h"
#include <KlemmUI/Application.h>
#include <iostream>
#include <cstring>
#include <GL/gl.h>

Display* KlemmUI::SystemWM::X11Window::XDisplay = nullptr;
::Window KlemmUI::SystemWM::X11Window::XRootWindow;

std::string GetEnv(const std::string& var)
{
	const char* val = std::getenv(var.c_str());

	if (val == nullptr)
		return "";

	return val;
}

void KlemmUI::SystemWM::X11Window::Create(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup)
{
	std::cout << "- [kui-X11]: $DISPLAY = '" << GetEnv("DISPLAY") << "'" << std::endl;

	if (XDisplay == nullptr)
		XDisplay = XOpenDisplay(NULL);

	if (!XDisplay)
	{
		Application::Error::Error("Failed to open x11 display", true);
		return;
	}

	XRootWindow = DefaultRootWindow(XDisplay);
	if (!XRootWindow)
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
	attrib.event_mask = ExposureMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
	XWindow = XCreateWindow(XDisplay, XRootWindow, Pos.X, Pos.Y, Size.X, Size.Y, 0, visual->depth, InputOutput, visual->visual, CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrib);

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

void KlemmUI::SystemWM::X11Window::Destroy()
{
}

void KlemmUI::SystemWM::X11Window::MakeContextCurrent() const
{
	if (glXGetCurrentContext() != GLContext)
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
		case FocusIn:
			HasFocus = true;
			break;
		case FocusOut:
			HasFocus = false;
			break;
		case DestroyNotify:
			Parent->Close();
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

bool KlemmUI::SystemWM::X11Window::IsLMBDown()
{
    return QueryPointer(nullptr) & Button1Mask;
}

int KlemmUI::SystemWM::X11Window::QueryPointer(Vector2ui *MousePos)
{
	::Window OutRoot;
	::Window OutChild;
	int RootX, RootY, WinX, WinY;
	unsigned int Mask;
	XQueryPointer(XDisplay, XRootWindow, &OutRoot, &OutChild,
		&RootX, &RootY, &WinX, &WinY, &Mask);
	
	if (MousePos)
	{
		MousePos->X = RootX;
		MousePos->Y = RootY;
	}

    return Mask;
}

#endif
