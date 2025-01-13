#include "kui/StringReplace.h"
using namespace kui;

void strReplace::ReplaceChar(std::string& Target, char A, std::string b)
{
	std::string NewStr = Target;
	for (size_t i = 0; i < Target.size(); i++)
	{
		if (Target[i] == A)
		{
			NewStr.erase(i, 1);
			NewStr.insert(i, b);
		}
	}
	Target = NewStr;
}