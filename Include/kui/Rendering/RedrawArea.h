#pragma once
#include <kui/Vec2.h>

namespace kui::render
{
	struct RedrawBox
	{
		Vec2f Min;
		Vec2f Max;

		static bool IsBoxOverlapping(const RedrawBox& BoxA, const RedrawBox& BoxB);

		static RedrawBox CombineBoxes(const RedrawBox& BoxA, const RedrawBox& BoxB)
		{
			return RedrawBox{
				.Min = Vec2f::Min(BoxA.Min, BoxB.Min),
				.Max = Vec2f::Max(BoxA.Max, BoxB.Max),
			};
		}
	};
}