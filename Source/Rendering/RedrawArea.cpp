#include <kui/Rendering/RedrawArea.h>

bool kui::render::RedrawBox::IsBoxOverlapping(const RedrawBox& BoxA, const RedrawBox& BoxB)
{
	return (BoxA.Min.X <= BoxB.Max.X && BoxA.Max.X >= BoxB.Min.X) &&
		(BoxA.Min.Y <= BoxB.Max.Y && BoxA.Max.Y >= BoxB.Min.Y);
}
