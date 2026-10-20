#pragma once
#include <kui/UI/UIBox.h>

namespace kui
{
	class UIStackBox : public UIBox
	{
	public:

		UIStackBox(Vec2f Position, SizeVec Size);

		Vec2f UpdateScale() override;
		void UpdateChildPosition() override;

	};
}