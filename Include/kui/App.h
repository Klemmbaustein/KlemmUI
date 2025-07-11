#pragma once
#include <string>
#include <functional>

/**
* @file
*/

/**
* @brief
* Containing functions for the application.
*/
namespace kui::app
{
	/**
	 * @brief
	 * Message type used in the message box function.
	 *
	 * @see MessageBox
	 */
	enum class MessageType
	{
		Info,
		Warn,
		Error,
	};


	// Backwards compatability
	using MessageBoxType = MessageType;

	/**
	 * @brief
	 * Creates a platform specific message box displaying a plain text message to the user.
	 *
	 * On Windows this uses the MessageBox windows API, on linux this will call either kdialog or zenity if they're available.
	 *
	 * @param Text
	 * The text displayed in the message box.
	 * @param Title
	 * The title of the message box, usually the title of the message box window.
	 * @param Type
	 * The type of the message box. This might add an icon next to the text and/or play an error sound if the type is error or warning.
	 */
	void MessageBox(std::string Text, std::string Title, MessageType Type);

	/**
	 * @brief
	 * Shows a yes/no dialog box
	 *
	 * On Windows this uses the MessageBox windows API, on linux this will call either kdialog or zenity if they're available.
	 *
	 * @param Text
	 * The title displayed in the dialog box.
	 * @param Title
	 * The title of the dialog, usually the title of the dialog window.
	 * @return
	 * True if the user chose yes, false if the user chose no.
	 */
	bool YesNoBox(std::string Text, std::string Title);

	/**
	 * @brief
	 * Creates a platform file picker dialog
	 *
	 * On Windows this uses the IFileOpenDialog COM interface, on linux this will call either kdialog or zenity if they're available.
	 *
	 * @param PickFolders
	 * True if the user should pick folders/directories instead of files.
	 * @return
	 * A string containing a path to the selected file, or an empty string if the selection was cancelled.
	 */
	std::string SelectFileDialog(bool PickFolders);

	/**
	 * @brief
	 * Application error handling.
	 */
	namespace error
	{
		/**
		 * @brief
		 * Function used for error messages.
		 *
		 * This function calls the function set with kui::app::error::SetErrorMessageCallback().
		 *
		 * @param Message
		 * The error message.
		 *
		 * @param Fatal
		 * If this is true, the program will crash after displaying the message.
		 */
		void Error(std::string Message, bool Fatal = false);

		/**
		 * @brief
		 * Sets the error message callback.
		 *
		 * The default callback will print the error message to standard output.
		 *
		 * On windows, you might want this message to call `MessageBox()` or something similar.
		 */
		void SetErrorCallback(std::function<void(std::string Message, bool IsFatal)> Callback);
	}
}