#include <KlemmUI/Application.h>
#include "Window/SystemWM.h"

static std::function<void(std::string Message, bool IsFatal)> ErrorCallback = 
	[](std::string Message, bool)
	{
		puts(Message.c_str());
	};

void KlemmUI::Application::MessageBox(std::string Text, std::string Title, MessageBoxType Type)
{
	SystemWM::MessageBox(Text, Title, int(Type));
}

void KlemmUI::Application::Error::Error(std::string Message, bool Fatal)
{
	ErrorCallback(Message, Fatal);
	if (Fatal)
	{
		abort();
	}
}

void KlemmUI::Application::Error::SetErrorCallback(std::function<void(std::string Message, bool IsFatal)> Callback)
{
	ErrorCallback = Callback;
}
