#include <KlemmUI/Application.h>
#include <iostream>
#include "Internal.h"
#if _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace KlemmUI::Application
{
	static std::string ShaderPath;

	static void(*ErrorCallback)(std::string Message) = [](std::string Message) {
		puts(Message.c_str());
		};
}

std::string KlemmUI::Application::GetShaderPath()
{
	return ShaderPath;
}

void KlemmUI::Application::SetShaderPath(std::string NewShaderPath)
{
	ShaderPath = NewShaderPath;
}

void KlemmUI::Application::Initialize(std::string ShaderPath)
{
#if _WIN32
	SetProcessDPIAware();
#endif

	SetShaderPath(ShaderPath);
	Internal::InitSDL();
}

void KlemmUI::Application::Error::Error(std::string Message, bool Fatal)
{
	ErrorCallback(Message);
	if (Fatal)
	{
		abort();
	}
}

void KlemmUI::Application::Error::SetErrorCallback(void(*Callback)(std::string Message))
{
	ErrorCallback = Callback;
}
