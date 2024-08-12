#pragma once
#include "Vector2.h"
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
namespace KlemmUI
{
	class Window;

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

#if 0
		NUMLOCKCLEAR = TO_KEYCODE(Key_Scancode::SCANCODE_NUMLOCKCLEAR),
		KP_DIVIDE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_DIVIDE),
		KP_MULTIPLY = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MULTIPLY),
		KP_MINUS = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MINUS),
		KP_PLUS = TO_KEYCODE(Key_Scancode::SCANCODE_KP_PLUS),
		KP_ENTER = 257,
		KP_1 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_1),
		KP_2 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_2),
		KP_3 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_3),
		KP_4 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_4),
		KP_5 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_5),
		KP_6 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_6),
		KP_7 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_7),
		KP_8 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_8),
		KP_9 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_9),
		KP_0 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_0),
		KP_PERIOD = TO_KEYCODE(Key_Scancode::SCANCODE_KP_PERIOD),

		APPLICATION = TO_KEYCODE(Key_Scancode::SCANCODE_APPLICATION),
		POWER = TO_KEYCODE(Key_Scancode::SCANCODE_POWER),
		KP_EQUALS = TO_KEYCODE(Key_Scancode::SCANCODE_KP_EQUALS),
		F13 = TO_KEYCODE(Key_Scancode::SCANCODE_F13),
		F14 = TO_KEYCODE(Key_Scancode::SCANCODE_F14),
		F15 = TO_KEYCODE(Key_Scancode::SCANCODE_F15),
		F16 = TO_KEYCODE(Key_Scancode::SCANCODE_F16),
		F17 = TO_KEYCODE(Key_Scancode::SCANCODE_F17),
		F18 = TO_KEYCODE(Key_Scancode::SCANCODE_F18),
		F19 = TO_KEYCODE(Key_Scancode::SCANCODE_F19),
		F20 = TO_KEYCODE(Key_Scancode::SCANCODE_F20),
		F21 = TO_KEYCODE(Key_Scancode::SCANCODE_F21),
		F22 = TO_KEYCODE(Key_Scancode::SCANCODE_F22),
		F23 = TO_KEYCODE(Key_Scancode::SCANCODE_F23),
		F24 = TO_KEYCODE(Key_Scancode::SCANCODE_F24),
		EXECUTE = TO_KEYCODE(Key_Scancode::SCANCODE_EXECUTE),
		HELP = TO_KEYCODE(Key_Scancode::SCANCODE_HELP),
		MENU = TO_KEYCODE(Key_Scancode::SCANCODE_MENU),
		SELECT = TO_KEYCODE(Key_Scancode::SCANCODE_SELECT),
		STOP = TO_KEYCODE(Key_Scancode::SCANCODE_STOP),
		AGAIN = TO_KEYCODE(Key_Scancode::SCANCODE_AGAIN),
		UNDO = TO_KEYCODE(Key_Scancode::SCANCODE_UNDO),
		CUT = TO_KEYCODE(Key_Scancode::SCANCODE_CUT),
		COPY = TO_KEYCODE(Key_Scancode::SCANCODE_COPY),
		PASTE = TO_KEYCODE(Key_Scancode::SCANCODE_PASTE),
		FIND = TO_KEYCODE(Key_Scancode::SCANCODE_FIND),
		MUTE = TO_KEYCODE(Key_Scancode::SCANCODE_MUTE),
		VOLUMEUP = TO_KEYCODE(Key_Scancode::SCANCODE_VOLUMEUP),
		VOLUMEDOWN = TO_KEYCODE(Key_Scancode::SCANCODE_VOLUMEDOWN),
		KP_COMMA = TO_KEYCODE(Key_Scancode::SCANCODE_KP_COMMA),
		KP_EQUALSAS400 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_EQUALSAS400),

		ALTERASE = TO_KEYCODE(Key_Scancode::SCANCODE_ALTERASE),
		SYSREQ = TO_KEYCODE(Key_Scancode::SCANCODE_SYSREQ),
		CANCEL = TO_KEYCODE(Key_Scancode::SCANCODE_CANCEL),
		CLEAR = TO_KEYCODE(Key_Scancode::SCANCODE_CLEAR),
		PRIOR = TO_KEYCODE(Key_Scancode::SCANCODE_PRIOR),
		RETURN2 = TO_KEYCODE(Key_Scancode::SCANCODE_RETURN2),
		SEPARATOR = TO_KEYCODE(Key_Scancode::SCANCODE_SEPARATOR),
		OUT = TO_KEYCODE(Key_Scancode::SCANCODE_OUT),
		OPER = TO_KEYCODE(Key_Scancode::SCANCODE_OPER),
		CLEARAGAIN = TO_KEYCODE(Key_Scancode::SCANCODE_CLEARAGAIN),
		CRSEL = TO_KEYCODE(Key_Scancode::SCANCODE_CRSEL),
		EXSEL = TO_KEYCODE(Key_Scancode::SCANCODE_EXSEL),

		KP_00 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_00),
		KP_000 = TO_KEYCODE(Key_Scancode::SCANCODE_KP_000),
		THOUSANDSSEPARATOR = TO_KEYCODE(Key_Scancode::SCANCODE_THOUSANDSSEPARATOR),
		DECIMALSEPARATOR = TO_KEYCODE(Key_Scancode::SCANCODE_DECIMALSEPARATOR),
		CURRENCYUNIT = TO_KEYCODE(Key_Scancode::SCANCODE_CURRENCYUNIT),
		CURRENCYSUBUNIT = TO_KEYCODE(Key_Scancode::SCANCODE_CURRENCYSUBUNIT),
		KP_LEFTPAREN = TO_KEYCODE(Key_Scancode::SCANCODE_KP_LEFTPAREN),
		KP_RIGHTPAREN = TO_KEYCODE(Key_Scancode::SCANCODE_KP_RIGHTPAREN),
		KP_LEFTBRACE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_LEFTBRACE),
		KP_RIGHTBRACE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_RIGHTBRACE),
		KP_TAB = TO_KEYCODE(Key_Scancode::SCANCODE_KP_TAB),
		KP_BACKSPACE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_BACKSPACE),
		KP_A = TO_KEYCODE(Key_Scancode::SCANCODE_KP_A),
		KP_B = TO_KEYCODE(Key_Scancode::SCANCODE_KP_B),
		KP_C = TO_KEYCODE(Key_Scancode::SCANCODE_KP_C),
		KP_D = TO_KEYCODE(Key_Scancode::SCANCODE_KP_D),
		KP_E = TO_KEYCODE(Key_Scancode::SCANCODE_KP_E),
		KP_F = TO_KEYCODE(Key_Scancode::SCANCODE_KP_F),
		KP_XOR = TO_KEYCODE(Key_Scancode::SCANCODE_KP_XOR),
		KP_POWER = TO_KEYCODE(Key_Scancode::SCANCODE_KP_POWER),
		KP_PERCENT = TO_KEYCODE(Key_Scancode::SCANCODE_KP_PERCENT),
		KP_LESS = TO_KEYCODE(Key_Scancode::SCANCODE_KP_LESS),
		KP_GREATER = TO_KEYCODE(Key_Scancode::SCANCODE_KP_GREATER),
		KP_AMPERSAND = TO_KEYCODE(Key_Scancode::SCANCODE_KP_AMPERSAND),
		KP_DBLAMPERSAND = TO_KEYCODE(Key_Scancode::SCANCODE_KP_DBLAMPERSAND),
		KP_VERTICALBAR = TO_KEYCODE(Key_Scancode::SCANCODE_KP_VERTICALBAR),
		KP_DBLVERTICALBAR = TO_KEYCODE(Key_Scancode::SCANCODE_KP_DBLVERTICALBAR),
		KP_COLON = TO_KEYCODE(Key_Scancode::SCANCODE_KP_COLON),
		KP_HASH = TO_KEYCODE(Key_Scancode::SCANCODE_KP_HASH),
		KP_SPACE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_SPACE),
		KP_AT = TO_KEYCODE(Key_Scancode::SCANCODE_KP_AT),
		KP_EXCLAM = TO_KEYCODE(Key_Scancode::SCANCODE_KP_EXCLAM),
		KP_MEMSTORE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMSTORE),
		KP_MEMRECALL = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMRECALL),
		KP_MEMCLEAR = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMCLEAR),
		KP_MEMADD = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMADD),
		KP_MEMSUBTRACT = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMSUBTRACT),
		KP_MEMMULTIPLY = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMMULTIPLY),
		KP_MEMDIVIDE = TO_KEYCODE(Key_Scancode::SCANCODE_KP_MEMDIVIDE),
		KP_PLUSMINUS = TO_KEYCODE(Key_Scancode::SCANCODE_KP_PLUSMINUS),
		KP_CLEAR = TO_KEYCODE(Key_Scancode::SCANCODE_KP_CLEAR),
		KP_CLEARENTRY = TO_KEYCODE(Key_Scancode::SCANCODE_KP_CLEARENTRY),
		KP_BINARY = TO_KEYCODE(Key_Scancode::SCANCODE_KP_BINARY),
		KP_OCTAL = TO_KEYCODE(Key_Scancode::SCANCODE_KP_OCTAL),
		KP_DECIMAL = TO_KEYCODE(Key_Scancode::SCANCODE_KP_DECIMAL),
		KP_HEXADECIMAL = TO_KEYCODE(Key_Scancode::SCANCODE_KP_HEXADECIMAL),
#endif
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
	 * See KlemmUI::Window::Input.
	 */
	class InputManager
	{
		std::map<Key, bool> PressedKeys;

		static Window* GetWindowByPtr(void* ID);
		std::map<Key, std::vector<void(*)(Window*)>> ButtonPressedCallbacks;
		Window* ParentWindow = nullptr;

		void MoveTextIndex(int Amount, bool RespectShiftPress = true);

		std::atomic<int> ScrollAmount = 0;

	public:
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

		Vector2ui GetMouseScreenPosition();

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

		bool CursorVisible = false;
		/**
		 * @brief
		 * The position of the moue cursor relative to the window.
		 * 
		 * -1, -1 is the bottom left corner of the screen and 1, 1 is the top right corner.
		 */
		Vector2f MousePosition = 100;

		bool PollForText = false;
		std::string Text;
		int TextIndex = 0;
		int TextSelectionStart = 0;

		/// Gets the string selected for text input.
		std::string GetSelectedTextString() const;

		void SetTextIndex(int NewIndex, bool ClearSelection);
	};
}
