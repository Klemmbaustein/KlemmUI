#pragma once
#include <kui/Vec3.h>

namespace kui
{
	class WindowColors
	{
	public:
		Vec3f ScrollBackgroundColor = 0.25f;
		Vec3f ScrollBackgroundBorderColor = 0.75f;
		Vec3f ScrollBarColor = 0.15f;

		Vec3f TextFieldSelection = Vec3f(0, 0.25f, 0.75f);
		Vec3f TextFieldTextDefaultColor = 1;

		Vec3f KeyboardSelectionColor = 1.0f;
	};
}