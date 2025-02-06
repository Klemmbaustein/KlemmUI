#pragma once
#include <string>
#include <vector>

namespace Format
{
	struct FormatArg
	{
		std::string Name;
		std::string Value;
	};

	std::string FormatString(std::string Format, std::vector<FormatArg> Args);
}