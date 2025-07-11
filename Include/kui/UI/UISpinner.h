#pragma once
#include "UIBackground.h"

namespace kui
{
	/**
	 * @brief
	 * A spinner element that shows an operation is going on in the background.
	 */
	class UISpinner : public UIBackground
	{
	protected:
		float Time = 0;
	public:
		UISpinner(Vec2f Position, Vec3f Color, SizeVec Size = SizeVec(40, SizeMode::PixelRelative));
		virtual ~UISpinner() override;

		float Speed = 1;
		Vec3f BackgroundColor = 0;

		/**
		 * @brief
		 * Sets the speed of the spinner animation
		 * @param NewSpeed
		 * The new speed of the animation
		 * @return
		 * A pointer to this.
		 */
		UISpinner* SetSpeed(float NewSpeed);

		/**
		 * @brief
		 * Sets the color of the spinner's background
		 * @param NewColor
		 * The color of the spinner's background.
		 * @return
		 * A pointer to this.
		 */
		UISpinner* SetBackgroundColor(Vec3f NewColor);

		void Tick() override;
		virtual void Draw() override;

		bool Active = true;
	};
}