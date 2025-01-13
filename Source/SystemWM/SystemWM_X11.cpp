#if __linux__ && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM.h"
#include "SystemWM_Linux.h"
#include "SystemWM_X11.h"
#include <kui/App.h>
#include <cstring>
#include <GL/gl.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <iostream>
#include <thread>

#ifdef KLEMMUI_USE_XRANDR
#include <X11/extensions/Xrandr.h>
#define HAS_XRANDR 1
#endif

thread_local Display* kui::systemWM::X11Window::XDisplay = nullptr;
thread_local ::Window kui::systemWM::X11Window::XRootWindow;

static std::string GetEnv(const std::string& var)
{
	const char* val = std::getenv(var.c_str());

	if (val == nullptr)
		return "";

	return val;
}

static void CheckForDisplay()
{
	using namespace kui::systemWM;
	using namespace kui::app::error;

	if (X11Window::XDisplay == nullptr)
	{
		X11Window::XDisplay = XOpenDisplay(NULL);
	}

	if (!X11Window::XDisplay)
	{
		Error("Failed to open x11 display", true);
		return;
	}

	X11Window::XRootWindow = DefaultRootWindow(X11Window::XDisplay);
	if (!X11Window::XRootWindow)
	{
		Error("No root window found", true);
		XCloseDisplay(X11Window::XDisplay);
		return;
	}
}

static unsigned int GetX11CursorFont(kui::Window::Cursor CursorType)
{
	switch (CursorType)
	{
		/* X Font Cursors reference: */
		/*   http://tronche.com/gui/x/xlib/appendix/b/ */
	case kui::Window::Cursor::Default: return XC_left_ptr;
	case kui::Window::Cursor::Text: return XC_xterm;
	case kui::Window::Cursor::Hand: return XC_hand2;
	case kui::Window::Cursor::ResizeLeftRight: return XC_sb_h_double_arrow;
	case kui::Window::Cursor::ResizeUpDown: return XC_sb_v_double_arrow;
	default:
		break;
	}
	return 0;
}

thread_local static Atom WmDeleteWindow;
thread_local static Atom NetWmStateMaximizedHorz;
thread_local static Atom NetWmStateMaximizedVert;
thread_local static Atom NetWmStateAbove;

namespace kui::systemWM::X11Borderless
{
	// Based on implementation of SDL2

	constexpr int HITRESULT_NONE = -2;
	constexpr int HITRESULT_DRAG = -1;
	constexpr int _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0;
	constexpr int _NET_WM_MOVERESIZE_SIZE_TOP = 1;
	constexpr int _NET_WM_MOVERESIZE_SIZE_TOPRIGHT = 2;
	constexpr int _NET_WM_MOVERESIZE_SIZE_RIGHT = 3;
	constexpr int _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT = 4;
	constexpr int _NET_WM_MOVERESIZE_SIZE_BOTTOM = 5;
	constexpr int _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT = 6;
	constexpr int _NET_WM_MOVERESIZE_SIZE_LEFT = 7;
	constexpr int _NET_WM_MOVERESIZE_MOVE = 8;

	static void MakeWindowBorderless(X11Window* Target, bool Border)
	{
		using namespace kui::systemWM;
		Atom WmHints = XInternAtom(Target->XDisplay, "_MOTIF_WM_HINTS", True);
		if (WmHints != None)
		{
			struct
			{
				unsigned long flags;
				unsigned long functions;
				unsigned long decorations;
				long input_mode;
				unsigned long status;
			} MWMHints = {
				0b10, 0, (unsigned long)(Border ? 1 : 0), 0, 0
			};

			XChangeProperty(Target->XDisplay, Target->XWindow, WmHints, WmHints, 32,
				PropModeReplace, (unsigned char*)&MWMHints,
				sizeof(MWMHints) / sizeof(long));
		}
		else
		{
			XSetTransientForHint(Target->XDisplay, Target->XWindow, RootWindow(Target->XDisplay, XDefaultScreen(Target->XDisplay)));
		}
	}

	static void InitiateWindowMove(X11Window* Target, const Vec2ui& Point)
	{
		XEvent evt;

		XUngrabPointer(Target->XDisplay, 0);
		XFlush(Target->XDisplay);
		Vec2ui Position = Target->GetPosition();

		evt.xclient.type = ClientMessage;
		evt.xclient.window = Target->XWindow;
		evt.xclient.message_type = XInternAtom(Target->XDisplay, "_NET_WM_MOVERESIZE", True);
		evt.xclient.format = 32;
		evt.xclient.data.l[0] = (size_t)Position.X + Point.X;
		evt.xclient.data.l[1] = (size_t)Position.Y + Point.Y;
		evt.xclient.data.l[2] = _NET_WM_MOVERESIZE_MOVE;
		evt.xclient.data.l[3] = Button1;
		evt.xclient.data.l[4] = 0;
		XSendEvent(Target->XDisplay, DefaultRootWindow(Target->XDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &evt);

		XSync(Target->XDisplay, 0);
	}

	static void InitiateWindowResize(X11Window* Target, const Vec2ui& Point, int direction)
	{
		XEvent evt;

		if (direction < _NET_WM_MOVERESIZE_SIZE_TOPLEFT || direction > _NET_WM_MOVERESIZE_SIZE_LEFT)
		{
			return;
		}

		XUngrabPointer(Target->XDisplay, 0);
		XFlush(Target->XDisplay);
		Vec2ui Position = Target->GetPosition();

		evt.xclient.type = ClientMessage;
		evt.xclient.window = Target->XWindow;
		evt.xclient.message_type = XInternAtom(Target->XDisplay, "_NET_WM_MOVERESIZE", True);
		evt.xclient.format = 32;
		evt.xclient.data.l[0] = (size_t)Position.X + Point.X;
		evt.xclient.data.l[1] = (size_t)Position.Y + Point.Y;
		evt.xclient.data.l[2] = direction;
		evt.xclient.data.l[3] = Button1;
		evt.xclient.data.l[4] = 0;
		XSendEvent(Target->XDisplay, DefaultRootWindow(Target->XDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &evt);

		XSync(Target->XDisplay, 0);
	}

	static int GetHitResult(X11Window* Target, Vec2ui CursorPosition)
	{
		const uint32_t BorderSize = 8;

		const int drag = Target->Parent->IsAreaGrabbableCallback
			&& Target->Parent->IsAreaGrabbableCallback(Target->Parent) ? HITRESULT_DRAG : HITRESULT_NONE;

		if (!Target->Resizable)
		{
			return drag;
		}

		enum region_mask
		{
			client = 0b0000,
			left = 0b0001,
			right = 0b0010,
			top = 0b0100,
			bottom = 0b1000,
		};

		Vec2ui Size = Target->GetSize();

		const int result =
			left * (CursorPosition.X < BorderSize) |
			right * (CursorPosition.X >= (Size.X - BorderSize)) |
			top * (CursorPosition.Y < BorderSize) |
			bottom * (CursorPosition.Y >= (Size.Y - BorderSize));

		switch (result)
		{
		case left: return _NET_WM_MOVERESIZE_SIZE_LEFT;
		case right: return _NET_WM_MOVERESIZE_SIZE_RIGHT;
		case top: return _NET_WM_MOVERESIZE_SIZE_TOP;
		case bottom: return _NET_WM_MOVERESIZE_SIZE_BOTTOM;
		case top | left: return _NET_WM_MOVERESIZE_SIZE_TOPLEFT;
		case top | right: return _NET_WM_MOVERESIZE_SIZE_TOPRIGHT;
		case bottom | left: return _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT;
		case bottom | right: return _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT;
		case client: return drag;
		default: return HITRESULT_NONE;
		}

	}

	static void ProcessHitTest(X11Window* Target, XEvent* xev)
	{
		const Vec2ui point = Vec2ui(xev->xbutton.x, xev->xbutton.y);

		const int result = GetHitResult(Target, point);

		if (result == HITRESULT_DRAG)
		{
			InitiateWindowMove(Target, point);
		}
		else if (result >= _NET_WM_MOVERESIZE_SIZE_TOPLEFT)
		{
			InitiateWindowResize(Target, point, result);
		}
	}

	static std::map<int, ::Cursor> LoadedCursors;

	static ::Cursor GetCursorFromHitResult(X11Window* Target, int Result)
	{
		if (LoadedCursors.contains(Result))
		{
			return LoadedCursors[Result];
		}

		::Cursor NewCursor = None;

		switch (Result)
		{
		case _NET_WM_MOVERESIZE_SIZE_LEFT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_left_side);
			break;
		case _NET_WM_MOVERESIZE_SIZE_RIGHT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_right_side);
			break;
		case _NET_WM_MOVERESIZE_SIZE_TOP:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_top_side);
			break;
		case _NET_WM_MOVERESIZE_SIZE_BOTTOM:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_bottom_side);
			break;
		case _NET_WM_MOVERESIZE_SIZE_TOPLEFT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_top_left_corner);
			break;
		case _NET_WM_MOVERESIZE_SIZE_TOPRIGHT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_top_right_corner);
			break;
		case _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_bottom_left_corner);
			break;
		case _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT:
			NewCursor = XCreateFontCursor(Target->XDisplay, XC_bottom_right_corner);
			break;
		default:
			break;
		}

		return NewCursor;
	}
}
void kui::systemWM::X11Window::Create(Window* Parent, Vec2ui Size, Vec2ui Pos, std::string Title,
	bool Borderless, bool Resizable, bool AlwaysOnTop)
{
	this->Borderless = Borderless;

	CheckForDisplay();

	GLint GlxAttributus[] =
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

	XVisualInfo* GlxVisual = glXChooseVisual(XDisplay, ScreenID, GlxAttributus);

	XSetWindowAttributes WindowAttributes;
	memset(&WindowAttributes, sizeof(WindowAttributes), 0);
	WindowAttributes.border_pixel = BlackPixel(XDisplay, ScreenID);
	WindowAttributes.override_redirect = True;
	WindowAttributes.colormap = XCreateColormap(XDisplay, RootWindow(XDisplay, ScreenID), GlxVisual->visual, AllocNone);
	WindowAttributes.event_mask = ExposureMask | FocusChangeMask | KeyPressMask
		| PointerMotionMask | KeyReleaseMask | SubstructureNotifyMask | ButtonPressMask;
	XWindow = XCreateWindow(XDisplay, XRootWindow, Pos.X, Pos.Y, Size.X, Size.Y, 0,
		GlxVisual->depth, InputOutput, GlxVisual->visual, CWColormap | CWBorderPixel | CWEventMask, &WindowAttributes);
	XStoreName(XDisplay, XWindow, Title.c_str());

	WmDeleteWindow = XInternAtom(XDisplay, "WM_DELETE_WINDOW", false);
	NetWmStateMaximizedHorz = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
	NetWmStateMaximizedVert = XInternAtom(XDisplay, "_NET_WM_STATE_MAXIMIZED_VERT", false);
	NetWmStateAbove = XInternAtom(XDisplay, "_NET_WM_STATE_ABOVE", false);

	if (None == XWindow)
	{
		app::error::Error("Failed to create window", true);
		XCloseDisplay(XDisplay);
		return;
	}
	InputMethod = XOpenIM(XDisplay, NULL, NULL, NULL);
	Input = XCreateIC(InputMethod, XNInputStyle, XIMStatusNothing | XIMPreeditNothing, XNClientWindow, XWindow, NULL);

	X11Borderless::MakeWindowBorderless(this, !Borderless);
	SetResizable(Resizable);
	SetAlwaysOnTop(AlwaysOnTop);

	XMapWindow(XDisplay, XWindow);

	if (GlxVisual == 0)
	{
		app::error::Error("Failed to create x11 visual", true);
		XCloseDisplay(XDisplay);
		return;
	}

	GLContext = glXCreateContext(XDisplay, GlxVisual, NULL, GL_TRUE);

	MakeContextCurrent();
}

void kui::systemWM::X11Window::Destroy()
{
	XDestroyWindow(XDisplay, XWindow);
	XFlush(XDisplay);
	XWindow = 0;
}

void kui::systemWM::X11Window::SetTitle(std::string NewTitle) const
{
	XStoreName(XDisplay, XWindow, NewTitle.c_str());
}

void kui::systemWM::X11Window::MakeContextCurrent() const
{
	if (glXGetCurrentContext() != GLContext)
		glXMakeCurrent(XDisplay, XWindow, GLContext);
}

// Based on: https://stackoverflow.com/questions/27378318/c-get-string-from-clipboard-on-linux/44992938#44992938
static std::string GetSelection(Display* display, Window window, const char* bufname, const char* fmtname)
{
	char* result = nullptr;
	unsigned long ressize, restail;
	int resbits;
	Atom bufid = XInternAtom(display, bufname, False),
		fmtid = XInternAtom(display, fmtname, False),
		propid = XInternAtom(display, "XSEL_DATA", False),
		incrid = XInternAtom(display, "INCR", False);
	XEvent event;

	XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
	do
	{
		XNextEvent(display, &event);
	} while (event.type != SelectionNotify || event.xselection.selection != bufid);

	std::string Result;
	if (event.xselection.property)
	{
		XGetWindowProperty(display, window, propid, 0, LONG_MAX / 4, True, AnyPropertyType,
			&fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
		if (fmtid != incrid)
			Result = std::string(result, ressize);
		XFree(result);

		if (fmtid == incrid)
			do
			{
				do
				{
					XNextEvent(display, &event);
				} while (event.type != PropertyNotify || event.xproperty.atom != propid || event.xproperty.state != PropertyNewValue);

				XGetWindowProperty(display, window, propid, 0, LONG_MAX / 4, True, AnyPropertyType,
					&fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
				Result = std::string(result, ressize);
				XFree(result);
			} while (ressize > 0);

		return Result;
	}
	return "";
}

void kui::systemWM::X11Window::UpdateWindow()
{
	while (XPending(XDisplay))
	{
		XEvent ev;
		XNextEvent(XDisplay, &ev);
		HandleEvent(ev);
	}
}

void kui::systemWM::X11Window::Swap() const
{
	glXSwapBuffers(XDisplay, XWindow);
}

bool kui::systemWM::X11Window::IsLMBDown()
{
	return QueryPointer(nullptr) & Button1Mask;
}

bool kui::systemWM::X11Window::IsRMBDown()
{
	return QueryPointer(nullptr) & Button3Mask;
}

void kui::systemWM::X11Window::SetCursor(Window::Cursor NewCursor)
{
	static std::map<Window::Cursor, Cursor> LoadedCursors;

	CurrentCursor = NewCursor;

	if (HoveringCorner)
	{
		return;
	}

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
		LoadedCursors.insert({ NewCursor, New });
		XDefineCursor(XDisplay, XWindow, New);
	}
}

void kui::systemWM::X11Window::SetMinSize(Vec2ui NewSize)
{
	MinSize = NewSize;
	XSizeHints* SizeHintsPtr = XAllocSizeHints();
	SizeHintsPtr->flags = PMinSize;
	SizeHintsPtr->min_width = NewSize.X;
	SizeHintsPtr->min_height = NewSize.Y;
	XSetWMNormalHints(XDisplay, XWindow, SizeHintsPtr);
	XFree(SizeHintsPtr);
}

void kui::systemWM::X11Window::SetMaxSize(Vec2ui NewSize)
{
	if (NewSize == 0)
		NewSize = INT16_MAX;

	MaxSize = NewSize;
	XSizeHints* SizeHintsPtr = XAllocSizeHints();
	SizeHintsPtr->flags = PMaxSize;
	SizeHintsPtr->max_width = NewSize.X;
	SizeHintsPtr->max_height = NewSize.Y;
	XSetWMNormalHints(XDisplay, XWindow, SizeHintsPtr);
	XFree(SizeHintsPtr);
}

void kui::systemWM::X11Window::Maximize() const
{
	XEvent xev;
	Atom wm_state = XInternAtom(XDisplay, "_NET_WM_STATE", False);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = XWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = NetWmStateMaximizedHorz;
	xev.xclient.data.l[2] = NetWmStateMaximizedVert;

	XSendEvent(XDisplay, DefaultRootWindow(XDisplay), False, SubstructureNotifyMask, &xev);
}

void kui::systemWM::X11Window::Minimize() const
{
	XIconifyWindow(XDisplay, XWindow, 0);
}

void kui::systemWM::X11Window::Restore() const
{
	XEvent xev;
	Atom wm_state = XInternAtom(XDisplay, "_NET_WM_STATE", false);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = XWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0;
	xev.xclient.data.l[1] = NetWmStateMaximizedHorz;
	xev.xclient.data.l[2] = NetWmStateMaximizedVert;

	XSendEvent(XDisplay, DefaultRootWindow(XDisplay), false, SubstructureNotifyMask, &xev);

	XClientMessageEvent ev;
	std::memset(&ev, 0, sizeof ev);
	ev.type = ClientMessage;
	ev.window = XWindow;
	ev.message_type = XInternAtom(XDisplay, "_NET_ACTIVE_WINDOW", True);
	ev.format = 32;
	ev.data.l[0] = 1;
	ev.data.l[1] = CurrentTime;
	ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
	XSendEvent(XDisplay, RootWindow(XDisplay, XDefaultScreen(XDisplay)), false,
		SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
	XFlush(XDisplay);
}

void kui::systemWM::X11Window::SetIcon(uint8_t* TextureBytes, size_t Width, size_t Height)
{
	std::vector<long> ArgbFormat;

	ArgbFormat.reserve(Width * Height + 2);
	ArgbFormat.push_back(Width);
	ArgbFormat.push_back(Height);

	for (size_t i = 0; i < Width * Height * 4; i += 4)
	{
		uint8_t Bytes[4];
		Bytes[0] = TextureBytes[i + 2]; // B
		Bytes[1] = TextureBytes[i + 1]; // G
		Bytes[2] = TextureBytes[i + 0]; // R
		Bytes[3] = TextureBytes[i + 3]; // A

		ArgbFormat.push_back(*reinterpret_cast<uint32_t*>(&Bytes));
	}

	Atom net_wm_icon = XInternAtom(XDisplay, "_NET_WM_ICON", false);
	Atom cardinal = XInternAtom(XDisplay, "CARDINAL", false);
	XChangeProperty(XDisplay, XWindow, net_wm_icon, cardinal, 32, PropModeReplace, (unsigned char*)ArgbFormat.data(), Width * Height + 2);
}

float kui::systemWM::X11Window::GetDPIScale()
{
	Screen* MainScreen = DefaultScreenOfDisplay(XDisplay);
	int Size = WidthOfScreen(MainScreen);
	int Width = WidthMMOfScreen(MainScreen);
	float Scale = float(Size) / float(Width) / 3.78f;

	// Make sure to round to a good value, instead of having a scale of 0.999 or similar.
	Scale = roundf(Scale * 10) / 10;

	return Scale;
}

bool kui::systemWM::X11Window::IsMaximized() const
{
	Atom ActualReturnType;
	int ActualReturnFormat;
	unsigned long BytesAfter;
	unsigned char* Returned;
	unsigned long ItemCount;
	int Result = XGetWindowProperty(XDisplay, XWindow, XInternAtom(XDisplay, "_NET_WM_STATE", False), 0, 1024, False, AnyPropertyType,
		&ActualReturnType, &ActualReturnFormat, &ItemCount, &BytesAfter, &Returned);

	if (Result != Success)
	{
		return false;
	}
	Atom* Atoms = (Atom*)Returned;

	for (int i = 0; i < ItemCount; i++)
	{
		if (Atoms[i] == NetWmStateMaximizedHorz)
		{
			XFree(Returned);
			return true;
		}
	}

	XFree(Returned);
	return false;
}

// I found no good way of doing this...
bool kui::systemWM::X11Window::IsMinimized()
{
	return false;
}

kui::Vec2ui kui::systemWM::X11Window::GetPosition() const
{
	XWindowAttributes xwa;
	XGetWindowAttributes(XDisplay, XWindow, &xwa);
	return Vec2ui(xwa.x, xwa.y);
}

kui::Vec2ui kui::systemWM::X11Window::GetSize() const
{
	XWindowAttributes xwa;
	XGetWindowAttributes(XDisplay, XWindow, &xwa);
	return Vec2ui(xwa.width, xwa.height);
}

void kui::systemWM::X11Window::SetPosition(Vec2ui NewPosition) const
{
	XMoveWindow(XDisplay, XWindow, NewPosition.X, NewPosition.Y);
}

void kui::systemWM::X11Window::SetSize(Vec2ui NewSize) const
{
	XResizeWindow(XDisplay, XWindow, NewSize.X, NewSize.Y);
}

std::string kui::systemWM::X11Window::GetClipboard()
{
	Window* Current = Window::GetActiveWindow();
	if (!Current)
		return "";

	SysWindow* CurrentWindow = static_cast<SysWindow*>(Current->GetSysWindow());

	return GetSelection(XDisplay, CurrentWindow->X11.XWindow, "CLIPBOARD", "STRING");
}

kui::Vec2ui kui::systemWM::X11Window::GetMainScreenResolution()
{
	CheckForDisplay();
#if HAS_XRANDR
	XRRScreenResources* screens = XRRGetScreenResources(XDisplay, DefaultRootWindow(XDisplay));
	XRRCrtcInfo* info = NULL;
	info = XRRGetCrtcInfo(XDisplay, screens, screens->crtcs[0]);
	Vec2ui Size = Vec2ui(info->width, info->height);
	XRRFreeCrtcInfo(info);
	XRRFreeScreenResources(screens);
	return Size;
#else
	int DefaultScreenIndex = DefaultScreen(XDisplay);
	Screen* DefaultScreenPtr = ScreenOfDisplay(XDisplay, DefaultScreenIndex);
	return Vec2ui(WidthOfScreen(DefaultScreenPtr), HeightOfScreen(DefaultScreenPtr));
#endif
}

uint32_t kui::systemWM::X11Window::GetMonitorRefreshRate() const
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

void kui::systemWM::X11Window::SetBorderless(bool NewBorderless)
{
	X11Borderless::MakeWindowBorderless(this, !NewBorderless);
	Borderless = NewBorderless;
}

void kui::systemWM::X11Window::SetResizable(bool NewResizable)
{
	if (NewResizable == Resizable)
		return;

	if (NewResizable)
	{
		SetMinSize(MinSize);
		SetMaxSize(MaxSize);
	}
	else
	{
		Vec2ui Size = GetSize();
		SetMinSize(Size);
		SetMaxSize(Size);
	}
	Resizable = NewResizable;
}

void kui::systemWM::X11Window::SetAlwaysOnTop(bool NewAlwaysOnTop)
{
	XClientMessageEvent xclient;
	memset(&xclient, 0, sizeof(xclient));

	xclient.type = ClientMessage;
	xclient.window = XWindow;
	xclient.message_type = XInternAtom(XDisplay, "_NET_WM_STATE", false);
	xclient.format = 32;
	xclient.data.l[0] = NewAlwaysOnTop ? 1 : 0;
	xclient.data.l[1] = NetWmStateAbove;
	xclient.data.l[2] = 0;
	xclient.data.l[3] = 1;
	xclient.data.l[4] = 0;

	XSendEvent(XDisplay,
		DefaultRootWindow(XDisplay), False,
		SubstructureRedirectMask | SubstructureNotifyMask,
		(XEvent*)&xclient);
	XSetWMProtocols(XDisplay, XWindow, &WmDeleteWindow, 1);
}

void kui::systemWM::X11Window::HandleEvent(XEvent ev)
{
	switch (ev.type)
	{
	case MotionNotify:
	{
		CursorPosition = Vec2i(ev.xmotion.x, ev.xmotion.y);
		int HitResult = X11Borderless::GetHitResult(this, CursorPosition);
		if (Resizable && Borderless && HitResult >= X11Borderless::_NET_WM_MOVERESIZE_SIZE_TOPLEFT)
		{
			if (HitResult != LastResizeCorner || HoveringCorner == false)
			{
				LastResizeCorner = HitResult;
				XDefineCursor(XDisplay, XWindow, X11Borderless::GetCursorFromHitResult(this, HitResult));
			}
			HoveringCorner = true;
		}
		else
		{
			if (HoveringCorner)
			{
				HoveringCorner = false;
				SetCursor(CurrentCursor);
			}
			HoveringCorner = false;
		}
		return;
	}
	case ButtonPress:
		if (Borderless)
			X11Borderless::ProcessHitTest(this, &ev);
		CursorPosition = Vec2i(ev.xbutton.x, ev.xbutton.y);
		return;
	case Expose:
	{
		Vec2ui NewSize = GetSize();
		if (WindowSize != NewSize)
		{
			WindowSize = NewSize;
			Parent->OnResized();
		}
		return;
	}
	case FocusIn:
		HasFocus = true;
		return;
	case FocusOut:
		HasFocus = false;
		return;
	case DestroyNotify:
		Parent->Close();
		return;
	case KeyPress:
	{
		KeySym Symbol = XLookupKeysym(&ev.xkey, 0);
		HandleKeyPress(Symbol, true);
		int UtfSize = 0;
		char UtfBuffer[32];
		Status StringLookupStatus = 0;
		UtfSize = Xutf8LookupString(Input, (XKeyPressedEvent*)&ev, UtfBuffer, sizeof(UtfBuffer) - 1, &Symbol, &StringLookupStatus);

		if (StringLookupStatus == XBufferOverflow)
			return;
		UtfBuffer[UtfSize] = 0;
		if (Symbol == XK_BackSpace)
			return;
		if (Symbol == XK_Delete)
			return;

		if (UtfSize)
		{
			TextInput.append(UtfBuffer);
		}

		return;
	}
	case KeyRelease:
	{
		KeySym Symbol = XLookupKeysym(&ev.xkey, 0);
		HandleKeyPress(Symbol, false);
		return;
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
		return;
	}
	std::cout << "Unknown event: " << ev.type << std::endl;
}

void kui::systemWM::X11Window::HandleKeyPress(KeySym Symbol, bool NewValue)
{
	static std::map<int, kui::Key> Keys =
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

int kui::systemWM::X11Window::QueryPointer(Vec2ui* MousePos)
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
