#pragma once
#include "MarkupStructure.h"
#include "WriteCode.h"
#include <fstream>

namespace kui::markup
{
	class HeaderWriter
	{
	public:

		struct HeaderElement
		{
			MarkupElement* Element;
			bool IsWritten = false;
			ElementWriter* Writer;
		};

		struct HeaderFile
		{
			std::string Name;
			std::string Path;

			std::vector<HeaderElement> Elements;

			void WriteElements(std::ofstream& Target, HeaderWriter* Writer);
		};

		HeaderWriter(ParseResult* Parsed);

		void Write(std::string Dir);

	private:
		std::map<std::string, HeaderFile> ToWrite;

		ParseResult* Parsed = nullptr;
	};
}