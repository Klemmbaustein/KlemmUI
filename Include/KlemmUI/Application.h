#pragma once
#include <string>

namespace KlemmUI::Application
{
	std::string GetShaderPath();
	void SetShaderPath(std::string NewShaderPath);

	namespace Error
	{
		void Error(std::string Message, bool Fatal = false);
	}
}