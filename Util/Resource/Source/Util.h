#include <string>
inline void ReplaceChar(std::string& Target, char A, std::string b)
{
	std::string From = { A };

	size_t start_pos = 0;
	while ((start_pos = Target.find(From, start_pos)) != std::string::npos)
	{
		Target.replace(start_pos, From.length(), b);
		start_pos += b.length();
	}
}
