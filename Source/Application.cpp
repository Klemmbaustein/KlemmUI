#include <KlemmUI/Application.h>
#include <iostream>
#include "Internal.h"

namespace KlemmUI::Application
{
	static std::string ShaderPath;
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
	SetShaderPath(ShaderPath);
	Internal::InitSDL();
}

void KlemmUI::Application::Error::Error(std::string Message, bool Fatal)
{
	std::puts(Message.c_str());
	if (Fatal)
	{
		abort();
	}
}
