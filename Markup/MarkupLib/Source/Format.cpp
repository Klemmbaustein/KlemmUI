#include "Markup/Format.h"
#include <iostream>

static void FormatError(std::string Message, std::string Format, size_t FormatChar)
{
	std::cout << "Error with format: " << Message << std::endl;
	std::cout << Format << std::endl;
	for (size_t i = 0; i < FormatChar; i++)
	{
		std::cout << " " << std::endl;
	}
	std::cout << "^ - here" << std::endl;
}

std::string Format::FormatString(std::string Format, std::vector<FormatArg> Args)
{

	std::string Out;
	std::string CurrentFormat;
	bool InFormat = false;

	size_t it = 0;
	for (char c : Format)
	{
		switch (c)
		{
		case '{':
			if (InFormat)
			{
				FormatError("Unexpected '{' in format", Format, it);
			}
			InFormat = true;
			break;
		case '}':
			if (!InFormat)
			{
				FormatError("Unexpected '}'.", Format, it);
			}

			for (auto& i : Args)
			{
				if (i.Name == CurrentFormat)
				{
					Out.append(i.Value);
					break;
				}
			}

			CurrentFormat.clear();
			InFormat = false;
			break;
		default:
			if (InFormat)
			{
				CurrentFormat.push_back(c);
			}
			else
			{
				Out.push_back(c);
			}
			break;
		}
		it++;
	}

	return Out;
}
