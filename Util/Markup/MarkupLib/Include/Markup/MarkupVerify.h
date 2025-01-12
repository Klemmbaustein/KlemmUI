#pragma once
#include "MarkupStructure.h"

namespace kui::markupVerify
{
	void VerifyElement(
		MarkupStructure::UIElement& Element,
		const MarkupStructure::MarkupElement& Root,
		MarkupStructure::ParseResult& Structure
	);

	void Verify(MarkupStructure::ParseResult& Structure);
}