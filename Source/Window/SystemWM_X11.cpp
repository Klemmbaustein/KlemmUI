#if __linux__
#include "SystemWM.h"
#include "SystemWM_X11.h"
#include <KlemmUI/Application.h>
#include <iostream>
#include <cstring>
#include <GL/gl.h>
#include <X11/cursorfont.h>

#ifdef KLEMMUI_USE_XRANDR
#include <X11/extensions/Xrandr.h>
#define HAS_XRANDR 1
#endif

#ifdef KLEMMUI_USE_XCURSOR
#include <X11/Xcursor/Xcursor.h>
#define HAS_XCURSOR 1
#endif

thread_local Display* KlemmUI::SystemWM::X11Window::XDisplay = nullptr;
thread_local ::Window KlemmUI::SystemWM::X11Window::XRootWindow;

std::string GetEnv(const std::string& var)
{
	const char* val = std::getenv(var.c_str());

	if (val == nullptr)
		return "";

	return val;
}

static void CheckForDisplay()
{
	using namespace KlemmUI::SystemWM;

	if (X11Window::XDisplay == nullptr)
	{
		std::cout << "- [kui-X11]: Opening X11 display: '" << GetEnv("DISPLAY") << "'" << std::endl;
		X11Window::XDisplay = XOpenDisplay(NULL);
	}

	if (!X11Window::XDisplay)
	{
		KlemmUI::Application::Error::Error("Failed to open x11 display", true);
		return;
	}

	X11Window::XRootWindow = DefaultRootWindow(X11Window::XDisplay);
	if (!X11Window::XRootWindow)
	{
		KlemmUI::Application::Error::Error("No root window found", true);
		XCloseDisplay(X11Window::XDisplay);
		return;
	}
}

static unsigned int GetX11CursorFont(KlemmUI::Window::Cursor CursorType)
{
	switch (CursorType)
	{
		/* X Font Cursors reference: */
		/*   http://tronche.com/gui/x/xlib/appendix/b/ */
	case KlemmUI::Window::Cursor::Default: return XC_left_ptr;
	case KlemmUI::Window::Cursor::Text: return XC_xterm;
	case KlemmUI::Window::Cursor::Hand: return XC_hand2;
	default:
		break;
	}
	return 0;
}

static Atom WmDeleteWindow;

void KlemmUI::SystemWM::X11Window::Create(Window* Parent, Vector2ui Size, Vector2ui Pos, std::string Title, bool Borderless, bool Resizable, bool Popup)
{
	CheckForDisplay();

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

	XVisualInfo* GlxVisual = glXChooseVisual(XDisplay, ScreenID, glxAttribs);

	XSetWindowAttributes WindowAttributes;
	memset(&WindowAttributes, sizeof(WindowAttributes), 0);
	WindowAttributes.border_pixel = BlackPixel(XDisplay, ScreenID);
	WindowAttributes.override_redirect = True;
	WindowAttributes.colormap = XCreateColormap(XDisplay, RootWindow(XDisplay, ScreenID), GlxVisual->visual, AllocNone);
	WindowAttributes.event_mask = ExposureMask | FocusChangeMask | KeyPressMask | PointerMotionMask | KeyReleaseMask | SubstructureNotifyMask | ButtonPressMask;
	XWindow = XCreateWindow(XDisplay, XRootWindow, Pos.X, Pos.Y, Size.X, Size.Y, 0, GlxVisual->depth, InputOutput, GlxVisual->visual, CWColormap | CWBorderPixel | CWEventMask, &WindowAttributes);
	XStoreName(XDisplay, XWindow, Title.c_str());
	WmDeleteWindow = XInternAtom(XDisplay, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(XDisplay, XWindow, &WmDeleteWindow, 1);

	if (None == XWindow)
	{
		Application::Error::Error("Failed to create window", true);
		XCloseDisplay(XDisplay);
		return;
	}
	InputMethod = XOpenIM(XDisplay, NULL, NULL, NULL);
	Input = XCreateIC(InputMethod, XNInputStyle, XIMStatusNothing | XIMPreeditNothing, XNClientWindow, XWindow, NULL);

	XMapWindow(XDisplay, XWindow);

	if (GlxVisual == 0)
	{
		Application::Error::Error("Failed to create x11 visual", true);
		XCloseDisplay(XDisplay);
		return;
	}

	GLContext = glXCreateContext(XDisplay, GlxVisual, NULL, GL_TRUE);

	MakeContextCurrent();
}

void KlemmUI::SystemWM::X11Window::Destroy()
{
	XDestroyWindow(XDisplay, XWindow);
	XFlush(XDisplay);
}

void KlemmUI::SystemWM::X11Window::SetTitle(std::string NewTitle) const
{
	XStoreName(XDisplay, XWindow, NewTitle.c_str());
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
			CursorPosition = Vector2i(ev.xmotion.x, ev.xmotion.y);
			continue;
		case ButtonPress:
			CursorPosition = Vector2i(ev.xbutton.x, ev.xbutton.y);
			continue;
		case Expose:
		{
			Vector2ui NewSize = Vector2ui(ev.xexpose.width, ev.xexpose.height);
			if (WindowSize != NewSize)
			{
				WindowSize = NewSize;
				Parent->OnResized();
			}
			continue;
		}
		case FocusIn:
			HasFocus = true;
			continue;
		case FocusOut:
			HasFocus = false;
			continue;
		case DestroyNotify:
			Parent->Close();
			continue;
		case KeyPress:
		{
			KeySym Symbol = XLookupKeysym(&ev.xkey, 0);
			HandleKeyPress(Symbol, true);
			int UtfSize = 0;
			char UtfBuffer[32];
			Status StringLookupStatus = 0;
			UtfSize = Xutf8LookupString(Input, (XKeyPressedEvent*)&ev, UtfBuffer, sizeof(UtfBuffer) - 1, &Symbol, &StringLookupStatus);

			if (StringLookupStatus == XBufferOverflow)
				continue;
			UtfBuffer[UtfSize] = 0;
			if (Symbol == XK_BackSpace)
				continue;
			if (Symbol == XK_Delete)
				continue;

			if (UtfSize)
			{
				TextInput.append(UtfBuffer);
			}

			continue;
		}
		case KeyRelease:
		{
			KeySym Symbol = XLookupKeysym(&ev.xkey, 0);
			HandleKeyPress(Symbol, false);
			continue;
		}
		default:
			break;
		}
		if ((Atom)ev.xclient.data.l[0] == WmDeleteWindow)
		{
			if (ev.xclient.window == XWindow)
			{
				Parent->Close();
			}
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

bool KlemmUI::SystemWM::X11Window::IsRMBDown()
{
	return QueryPointer(nullptr) & Button3Mask;
}

void KlemmUI::SystemWM::X11Window::SetCursor(Window::Cursor NewCursor) const
{
	static std::map<Window::Cursor, Cursor> LoadedCursors;

	if (NewCursor == Window::Cursor::Default)
	{
		XUndefineCursor(XDisplay, XWindow);
		return;
	}

	if (LoadedCursors.contains(NewCursor))
		XDefineCursor(XDisplay, XWindow, LoadedCursors[NewCursor]);
	else
	{
		Cursor New = XCreateFontCursor(XDisplay, GetX11CursorFont(NewCursor));
		LoadedCursors.insert({NewCursor, New});
		XDefineCursor(XDisplay, XWindow, New);
	}
}

void KlemmUI::SystemWM::X11Window::SetMinSize(Vector2ui NewSize)
{
	XSizeHints* SizeHintsPtr = XAllocSizeHints();
	SizeHintsPtr->flags = PMinSize;
	SizeHintsPtr->min_width = NewSize.X;
	SizeHintsPtr->min_height = NewSize.Y;
	XSetWMNormalHints(XDisplay, XWindow, SizeHintsPtr);
	XFree(SizeHintsPtr);
}

void KlemmUI::SystemWM::X11Window::SetMaxSize(Vector2ui NewSize)
{
	XSizeHints* SizeHintsPtr = XAllocSizeHints();
	SizeHintsPtr->flags = PMaxSize;
	SizeHintsPtr->max_width = NewSize.X;
	SizeHintsPtr->max_height = NewSize.Y;
	XSetWMNormalHints(XDisplay, XWindow, SizeHintsPtr);
	XFree(SizeHintsPtr);
}

void KlemmUI::SystemWM::X11Window::Maximize()
{
	XEvent xev;
	Atom wm_state = XInternAtom(XDisplay, "_NET_WM_STATE", False);
	Atom max_horz = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom max_vert = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = XWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = max_horz;
	xev.xclient.data.l[2] = max_vert;

	XSendEvent(XDisplay, DefaultRootWindow(XDisplay), False, SubstructureNotifyMask, &xev);
}

void KlemmUI::SystemWM::X11Window::Minimize()
{
	XIconifyWindow(XDisplay, XWindow, 0);
}

void KlemmUI::SystemWM::X11Window::Restore()
{
	XEvent xev;
	Atom wm_state = XInternAtom(XDisplay, "_NET_WM_STATE", False);
	Atom max_horz = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom max_vert = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = XWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = max_horz;
	xev.xclient.data.l[2] = max_vert;

	XSendEvent(XDisplay, DefaultRootWindow(XDisplay), False, SubstructureNotifyMask, &xev);

	XClientMessageEvent ev;
	std::memset(&ev, 0, sizeof ev);
	ev.type = ClientMessage;
	ev.window = XWindow;
	ev.message_type = XInternAtom(XDisplay, "_NET_ACTIVE_WINDOW", True);
	ev.format = 32;
	ev.data.l[0] = 1;
	ev.data.l[1] = CurrentTime;
	ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
	XSendEvent(XDisplay, RootWindow(XDisplay, XDefaultScreen(XDisplay)), False,
		SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
	XFlush(XDisplay);
}

Vector2ui KlemmUI::SystemWM::X11Window::GetPosition()
{
	XWindowAttributes xwa;
	XGetWindowAttributes(XDisplay, XWindow, &xwa);
	return Vector2ui(xwa.x, xwa.y);
}

Vector2ui KlemmUI::SystemWM::X11Window::GetSize()
{
	XWindowAttributes xwa;
	XGetWindowAttributes(XDisplay, XWindow, &xwa);
	return Vector2ui(xwa.width, xwa.height);
}

void KlemmUI::SystemWM::X11Window::SetPosition(Vector2ui NewPosition)
{
	XMoveWindow(XDisplay, XWindow, NewPosition.X, NewPosition.Y);
}

void KlemmUI::SystemWM::X11Window::SetSize(Vector2ui NewSize)
{
	XResizeWindow(XDisplay, XWindow, NewSize.X, NewSize.Y);
}

Vector2ui KlemmUI::SystemWM::X11Window::GetMainScreenResolution()
{
	CheckForDisplay();
#if HAS_XRANDR
	XRRScreenResources* screens = XRRGetScreenResources(XDisplay, DefaultRootWindow(XDisplay));
	XRRCrtcInfo* info = NULL;
	info = XRRGetCrtcInfo(XDisplay, screens, screens->crtcs[0]);
	Vector2ui Size = Vector2ui(info->width, info->height);
	XRRFreeCrtcInfo(info);
	XRRFreeScreenResources(screens);
	return Size;
#else
	int DefaultScreenIndex = DefaultScreen(XDisplay);
	Screen* DefaultScreenPtr = ScreenOfDisplay(XDisplay, DefaultScreenIndex);
	return Vector2ui(WidthOfScreen(DefaultScreenPtr), HeightOfScreen(DefaultScreenPtr));
#endif
}

uint32_t KlemmUI::SystemWM::X11Window::GetMonitorRefreshRate() const
{
#if HAS_XRANDR
	XRRScreenConfiguration* Config = XRRGetScreenInfo(XDisplay, XWindow);
	short Rate = XRRConfigCurrentRate(Config);
	XRRFreeScreenConfigInfo(Config);
	if (Rate == 0)
		return 60;
	return uint32_t(Rate);
#else
	return 60;
#endif
}

void KlemmUI::SystemWM::X11Window::HandleKeyPress(KeySym Symbol, bool NewValue)
{
	static std::map<int, KlemmUI::Key> Keys =
	{
		{XK_Escape, Key::ESCAPE},
		{XK_BackSpace, Key::BACKSPACE},
		{XK_Tab, Key::TAB},
		{XK_space, Key::SPACE},
		{XK_plus, Key::PLUS},
		{XK_comma, Key::COMMA},
		{XK_period, Key::PERIOD},
		{XK_slash, Key::SLASH},
		{XK_0, Key::k0},
		{XK_1, Key::k1},
		{XK_2, Key::k2},
		{XK_3, Key::k3},
		{XK_4, Key::k4},
		{XK_5, Key::k5},
		{XK_6, Key::k6},
		{XK_7, Key::k7},
		{XK_8, Key::k8},
		{XK_9, Key::k9},
		{XK_semicolon, Key::SEMICOLON},
		{XK_less, Key::LESS},
		{XK_Return, Key::RETURN},
		{XK_Return, Key::RETURN},
		{XK_bracketleft, Key::LEFTBRACKET},
		{XK_bracketright, Key::RIGHTBRACKET},
		{XK_Right, Key::RIGHT},
		{XK_Left, Key::LEFT},
		{XK_Up, Key::UP},
		{XK_Down, Key::DOWN},
		{XK_Shift_L, Key::LSHIFT},
		{XK_Shift_R, Key::RSHIFT},
		{XK_Control_L, Key::LCTRL},
		{XK_Control_R, Key::RCTRL},
		{XK_Alt_L, Key::LALT},
		{XK_Alt_R, Key::RALT},
		{XK_Delete, Key::DELETE},
		{XK_a, Key::a},
		{XK_b, Key::b},
		{XK_c, Key::c},
		{XK_d, Key::d},
		{XK_e, Key::e},
		{XK_f, Key::f},
		{XK_g, Key::g},
		{XK_h, Key::h},
		{XK_i, Key::i},
		{XK_j, Key::j},
		{XK_k, Key::k},
		{XK_l, Key::l},
		{XK_m, Key::m},
		{XK_n, Key::n},
		{XK_o, Key::o},
		{XK_p, Key::p},
		{XK_q, Key::q},
		{XK_r, Key::r},
		{XK_s, Key::s},
		{XK_t, Key::t},
		{XK_u, Key::u},
		{XK_v, Key::v},
		{XK_w, Key::w},
		{XK_x, Key::x},
		{XK_y, Key::y},
		{XK_z, Key::z},
	};

	if (!Keys.contains(Symbol))
		return;
	Parent->Input.SetKeyDown(Keys[Symbol], NewValue);
}

int KlemmUI::SystemWM::X11Window::QueryPointer(Vector2ui* MousePos)
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
