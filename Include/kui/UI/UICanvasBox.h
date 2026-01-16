#pragma once
#include "UIBox.h"

namespace kui
{
	class UICanvasBox : public UIBox
	{
	public:
		UICanvasBox(Vec2f Position, UISize Size);

		Vec2f UpdateScale() override;
		void UpdateChildPosition() override;
	};
}