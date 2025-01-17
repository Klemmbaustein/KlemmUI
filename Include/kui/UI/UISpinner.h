#pragma once
#include "UIBackground.h"

namespace kui
{
	class UISpinner : public UIBackground
	{
	protected:
		float Time = 0;
	public:
		UISpinner(Vec2f Position, Vec3f Color, SizeVec Size = SizeVec(40, SizeMode::PixelRelative));
		virtual ~UISpinner() override;

		float Speed = 1;
		Vec3f BackgroundColor = 0;
		UISpinner* SetSpeed(float NewSpeed);
		UISpinner* SetBackgroundColor(Vec3f NewColor);

		void Tick() override;
		virtual void Draw() override;

		bool Active = true;
	};
}