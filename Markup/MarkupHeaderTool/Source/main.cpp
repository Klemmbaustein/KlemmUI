#include <Markup/MarkupStructure.h>
#include "Markup/MarkupParse.h"
#include <fstream>

using namespace KlemmUI::MarkupStructure;

int main()
{
	std::ifstream Source = std::ifstream("D:/VS/KlemmUIML/Sheets/test.kui");

	std::string SourceString = std::string(std::istreambuf_iterator<char>(Source.rdbuf()),
		std::istreambuf_iterator<char>());

	auto a = KlemmUI::MarkupParse::ParseFiles({SourceString});

	MarkupElement e;
	e.Root = UIElement{
		.TypeName = "test",
		.Type = UIElement::ElementType::UserDefined,
		.Children = {
			UIElement{
			.TypeName = "UIBox",
			.Children = {UIElement{
				.TypeName = "UIText",
				.ElementProperties = {
					Property("text", "Hello, World!"),
					Property("size", "20"),
					Property("sizeMode", "PixelRelative"),
					Property("color", "1, 0, 0"),
					Property("font", "")
				}
			}},
			.ElementProperties = {
				Property("allAlign", "Centered"),
				Property("position", "-1"),
				Property("size", "2")
			},
		}}
	};

	e.WriteHeader("D:/VS/KlemmUIML/Source/Elements");
	if (a.size())
		a[0].WriteHeader("D:/VS/KlemmUIML/Source/Elements");
}
