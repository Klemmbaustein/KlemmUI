#pragma once
#include "MarkupStructure.h"

namespace kui::markupVerify
{
	void VerifyElement(
		MarkupStructure::UIElement& Element,
		MarkupStructure::MarkupElement& Root,
		MarkupStructure::ParseResult& Structure
	);

	void Verify(MarkupStructure::ParseResult& Structure);
}