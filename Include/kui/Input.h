#pragma once
#include "Vec2.h"
#include <vector>
#include <map>
#include <atomic>

/**
 * @file
 */

/**
 * @brief
 * Namespace containing all KlemmUI functions.
 */
namespace kui
{
	class Window;
	class UIBox;

	/**
	* @brief
	* Input keycode.
	*/
	enum class Key
	{
		UNKNOWN = 0,

		ESCAPE = 256,
		BACKSPACE = 259,
		TAB = '\t',
		SPACE = ' ',
		PLUS = '+',
		COMMA = ',',
		MINUS = '-',
		PERIOD = '.',
		SLASH = '/',
		k0 = '0',
		k1 = '1',
		k2 = '2',
		k3 = '3',
		k4 = '4',
		k5 = '5',
		k6 = '6',
		k7 = '7',
		k8 = '8',
		k9 = '9',
		SEMICOLON = ';',
		LESS = '<',
		RETURN = 257,

		/*
		   Skip uppercase letters
		 */

		LEFTBRACKET = '[',
		BACKSLASH = '\\',
		RIGHTBRACKET = ']',
		UNDERSCORE = '_',
		BACKQUOTE = '`',
		a = 'A',
		b = 'B',
		c = 'C',
		d = 'D',
		e = 'E',
		f = 'F',
		g = 'G',
		h = 'H',
		i = 'I',
		j = 'J',
		k = 'K',
		l = 'L',
		m = 'M',
		n = 'M',
		o = 'O',
		p = 'P',
		q = 'Q',
		r = 'R',
		s = 'S',
		t = 'T',
		u = 'U',
		v = 'V',
		w = 'W',
		x = 'X',
		y = 'Y',
		z = 'Z',

		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,

		INSERT = 260,
		HOME = 268,
		PAGEUP = 266,
		DELETE = 261,
		END = 269,
		PAGEDOWN = 267,
		RIGHT = 262,
		LEFT = 263,
		DOWN = 264,
		UP = 265,

		LCTRL = 341,
		LSHIFT = 340,
		LALT = 342,
		RCTRL = 345,
		RSHIFT = 344,
		RALT = 346,
	};

	/**
	 * @brief
	 * An input manager, handling input for a window.
	 * 
	 * See kui::Window::Input.
	 */
	class InputManager
	{

		static Window* GetWindowByPtr(void* ID);
		std::map<Key, std::vector<void(*)(Window*)>> ButtonPressedCallbacks;
		Window* ParentWindow = nullptr;

		void MoveTextIndex(int Amount, bool RespectShiftPress = true);

		std::atomic<int> ScrollAmount = 0;

	public:
		std::map<Key, bool> PressedKeys;
		InputManager(Window* ParentWindow);

		void UpdateCursorPosition();
		void Poll();
		void MoveMouseWheel(int Amount);

		/// Inserts the given string to the current text input.
		void AddTextInput(std::string Str);
		void DeleteTextSelection();

		/**
		 * @brief
		 * Checks if the given key is pressed.
		 * 
		 * @param PressedKey
		 * The key that should be checked.
		 * 
		 * @return
		 * True if the key is pressed, false if not.
		 */
		bool IsKeyDown(Key PressedKey);
		void SetKeyDown(Key PressedKey, bool KeyDown);

		Vec2ui GetMouseScreenPosition();

		/**
		 * @brief
		 * Adds a callback that will be run of the given key is pressed.
		 */
		void RegisterOnKeyDownCallback(Key PressedKey, void (*Callback)(Window*));
		/**
		 * @brief
		 * Removes a callback registered with RegisterOnKeyDownCallback().
		 */
		void RemoveOnKeyDownCallback(Key PressedKey, void (*Callback)(Window*));

		/// True if the left mouse button is pressed.
		bool IsLMBDown = false;
		/// True if the right mouse button is pressed.
		bool IsRMBDown = false;

		/// True if the left mouse has been pressed on the last update.
		bool IsLMBClicked = false;
		/// True if the right mouse has been pressed on the last update.
		bool IsRMBClicked = false;

		bool KeyboardFocusInput = true;

		kui::UIBox* KeyboardFocusTargetBox = nullptr;

		/**
		 * @brief
		 * The position of the moue cursor relative to the window.
		 * 
		 * -1, -1 is the bottom left corner of the screen and 1, 1 is the top right corner.
		 */
		Vec2f MousePosition = 100;

		bool PollForText = false;
		bool CanEditText = false;
		bool TextAllowNewLine = false;
		bool CursorVisible = false;
		std::string Text;
		int TextIndex = 0;
		int TextSelectionStart = 0;

		/// Gets the string selected for text input.
		std::string GetSelectedTextString() const;

		void SetTextIndex(int NewIndex, bool ClearSelection);
	};
}
