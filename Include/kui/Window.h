#pragma once
#include <string>
#include "Vec2.h"
#include "Input.h"
#include "Timer.h"
#include <vector>
#include <atomic>
#include "Rendering/ShaderManager.h"
#include "UI/UIManager.h"
#include "Markup/Markup.h"
#include <functional>

namespace kui
{
	class UIBox;
	
	/**
	* @brief
	* A KlemmUI window.
	* 
	* Every new UI element will be added to the active window. By default, the active window is the window that was last updated or created.
	* The active window can be set using the kui::Window::SetWindowActive() function.
	* The active window is thread local. On a different thread, another window might be active.
	* 
	* Everything related to a window should be done on the thread it was created on.
	*/
	class Window
	{
		void* SysWindowPtr = nullptr;

		Vec2ui WindowSize;

		void UpdateSize();
		Timer WindowDeltaTimer;

		float FrameDelta = 0;

		void WaitFrame();

		std::atomic<bool> ShouldClose = false;
		std::atomic<bool> ShouldUpdateSize = false;
		bool IsMainWindow = false;
		bool IgnoreDPI = false;
		static std::vector<Window*> ActiveWindows;
		Vec2ui MinSize;
		Vec2ui MaxSize;

	public:
		void RedrawInternal();

		/**
		 * @brief
		 * Sets this window as the active window.
		 * 
		 * Any new UI elements will be added to the active window.
		 */
		void SetWindowActive();

		void OnResized();

		/**
		* @brief
		* Sets a minimum size for the window.
		*/
		void SetMinSize(Vec2ui MinimumSize);
		/**
		* @brief
		* Sets a maximum size for the window.
		*/
		void SetMaxSize(Vec2ui MaximumSize);

		/**
		* @brief
		* Gets a list of all currently active windows.
		*/
		static std::vector<Window*> GetActiveWindows();

		/**
		 * @brief
		 * Either maximizes or restores the window.
		 */
		void SetMaximized(bool NewIsFullScreen);

		/// Returns if the window is maximized.
		bool GetWindowFullScreen();

		void SetMinimized(bool NewIsMinimized);
		bool GetMinimized();

		/**
		* @brief
		* Target framerate of the window.
		* A value of 0 uses the framerate of the window's monitor.
		*/
		uint32_t TargetFPS = 0;

		/**
		* @brief
		* The time elapsed since the Window was created.
		*/
		float Time = 0;

		/**
		* @brief
		* Closes the window.
		* 
		* This function can be called from any thread.
		*/
		void Close();

		/**
		* @brief
		* Returns the time between the last two calls to UpdateWindow.
		*/
		float GetDeltaTime() const;

		/// The input manager of this window.
		InputManager Input = InputManager(this);
		/// The shader manager of this window.
		ShaderManager Shaders;
		/// The UI manager of this window.
		UIManager UI;
		/// The markup manager of this window.
		MarkupLanguageManager Markup;

		/// Centered window position.
		static const Vec2ui POSITION_CENTERED;

		/// Default window size. Window has 60% of the screen's resolution.
		static const Vec2ui SIZE_DEFAULT;

		/**
		* @brief
		* Flags for window construction.
		*/
		enum class WindowFlag : int
		{
			///No window flags.
			None           = 0b000000,
			/// Borderless window.
			Borderless     = 0b000001,
			/// The window is resizable.
			Resizable      = 0b000010,
			/// The window should appear on top of all other windows.
			AlwaysOnTop    = 0b000100,
			/// The window should start maximized.
			FullScreen     = 0b001000,
			/// The window is a popup. A popup window will only have a close button.
			Popup          = 0b010000,
			/// The window ignores the system's DPI settings and will always have the same pixel size.
			IgnoreDPI      = 0b100000,
		};

		/**
		* @brief
		* Constructs a window with the given parameters
		*/
		Window(std::string Name, WindowFlag Flags, Vec2ui WindowPos = POSITION_CENTERED, Vec2ui WindowSize = SIZE_DEFAULT);
		Window(const Window&) = delete;
		virtual ~Window();

		/**
		 * @brief
		 * Gets the currently active window.
		 * 
		 * The active window is thread-local. Each thread can have a different active window.
		 */
		static Window* GetActiveWindow();

		/**
		 * @brief
		 * Gets a pointer to the internal systemWM::SysWindow structure.
		 */
		void* GetSysWindow() const;

		/**
		* @brief
		* Updates the current window, re-draws it if necessary.
		* 
		* Usage example:
		* ```cpp
		* while (MyWindow.UpdateWindow())
		* {
		*     // ...
		* }
		* ```
		* 
		* @return
		* Returns true if the window should continue being shown, false if not.
		*/
		bool UpdateWindow();

		/**
		* @brief
		* Gets the window's aspect ratio.
		* 
		* Aspect ratio = Width / Height.
		*/
		float GetAspectRatio() const;

		bool HasFocus();

		/**
		* @brief
		* Gets the window size in pixels.
		*/
		Vec2ui GetSize() const;

		void SetSize(Vec2ui NewSize);

		/**
		 * @brief
		 * Sets the position of the window.
		 * 
		 * @param Pos
		 * The position relative to the desktop, in pixels.
		 */
		void SetPosition(Vec2ui Pos);

		/**
		 * @brief
		 * Changes the icon used by the window to the given file.
		 */
		void SetIconFile(std::string IconFilePath);

		/**
		* @brief
		* The grabbable callback.
		* 
		* For a borderless window, the window manager has to know which area is grabbable by the mouse cursor.
		* This function should return true if the area currently hovered is grabbable and false if it's not.
		*/
		std::function<bool(Window*)> IsAreaGrabbableCallback = nullptr;
		/**
		 * @brief
		 * The resized callback.
		 * 
		 * This function is called once the window has been resized.
		 */
		std::function<void(Window*)> OnResizedCallback = nullptr;

		/**
		* @brief
		* Sets the window flags.
		*/
		void SetWindowFlags(WindowFlag NewFlags);
		WindowFlag GetWindowFlags() const;

		/**
		 * @brief
		 * @internal Makes the OpenGL context of this window active.
		 */
		void MakeContextCurrent();

		/**
		 * @brief
		 * Cancels the close operation of this window.
		 * 
		 * If the window has previously received a close message (From the Window::Close() function or if the user closed the window), it will be discarded.
		 */
		void CancelClose();

		/**
		 * @brief
		 * The border color used when the window is borderless.
		 */
		Vec3f BorderColor = 1;

		/**
		 * @brief
		 * System mouse cursor types.
		 */
		enum class Cursor
		{
			/// Default cursor.
			Default,
			/// "Hand" cursor, used when hovering a button.
			Hand,
			/// I-Beam cursor, used when hovering text.
			Text,
			ResizeUpDown,
			ResizeLeftRight,
			/// Cursor end index. Not an actual cursor.
			End
		};
		
		/// The active cursor for this window. 
		Cursor CurrentCursor = Cursor::Default;

		/**
		 * @brief
		 * Sets the window's title.
		 */
		void SetTitle(std::string NewTitle);
		
		/**
		 * @brief
		 * The multiplier value of the DPI.
		 * 
		 * The value returned by GetDPI() is multiplied with this. If the actual DPI scale is 1.5 and the DPI multiplier is 2, GetDPI() returns 3.
		 */
		float DPIMultiplier = 1;

		/**
		 * @brief
		 * Gets the DPI scale of the window.
		 * 
		 * The DPI scale is 1 by default (if the display scale in Windows is 100%, for example).
		 * If the screen DPI is larger, this value will increase as well.
		 * (On Windows: 150% display scale -> 1.5 DPI scale, 200% display scale -> 2.0 DPI)
		 */
		float GetDPI() const;

		void* GetPlatformHandle() const;

	private:
		float DPI = 1;
		void UpdateDPI();
		void HandleCursor();
		void* Cursors[(int)Cursor::End];
		WindowFlag CurrentWindowFlags;
	};

	Window::WindowFlag operator|(Window::WindowFlag a, Window::WindowFlag b);
	Window::WindowFlag operator&(Window::WindowFlag a, Window::WindowFlag b);
}