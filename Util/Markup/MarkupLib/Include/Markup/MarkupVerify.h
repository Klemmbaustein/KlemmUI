#pragma once
#include "MarkupStructure.h"

namespace kui::markup
{
	void VerifyElement(
		markup::UIElement& Element,
		markup::MarkupElement& Root,
		markup::ParseResult& Structure
	);

	void Verify(markup::ParseResult& Structure);
}