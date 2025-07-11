#pragma once
#include "Vec2.h"
#include <cmath>

namespace kui
{
	class Window;

	/**
	 * @brief
	 * Describes the way size should be calculated for a UIBox.
	 */
	enum class SizeMode
	{
		/**
		 * @brief
		 * Default value. Size should be relative to the screen.
		 *
		 * A box with the position x=-1, y=-1 (bottom left corner) and a size of x=1, y=2 will always fill half the screen.
		 *
		 * A box where `size x` = `size y` will only be square if the screen itself is square.
		 */
		ScreenRelative = 0,
		/**
		 * @brief
		 * Size should be relative to the aspect ratio.
		 *
		 * A box where `size x` = `size y` is guaranteed to be square.
		 *
		 * An AspectRelative box with the size x=1, x=1 has the same size as a ScreenRelative box with the size x=1/AspectRatio, y=1.
		 */
		AspectRelative = 1,
		/**
		 * @brief
		 * Size should be relative to the screen resolution.
		 *
		 * A box where `size x` = `size y` is guaranteed to be square.
		 * No matter the screens resolution, the box will always have the same size in pixels.
		 */
		PixelRelative = 2,
		/**
		* @brief
		* The size should be relative to the parent.
		*/
		ParentRelative = 3,
	};

	struct UISize
	{
		static Window* GetActiveWindow();

		float Value = 0;
		SizeMode Mode = SizeMode::ScreenRelative;

		UISize()
		{

		}

		UISize(float Val)
		{
			Value = Val;
		}

		UISize(float Val, SizeMode Mode)
		{
			Value = Val;
			this->Mode = Mode;
		}

		bool operator==(const UISize& b) const;

		static UISize Pixels(float px);

		static UISize Aspect(float ar)
		{
			UISize New;
			New.Mode = SizeMode::AspectRelative;
			New.Value = ar;
			return New;
		}

		static UISize Screen(float sr)
		{
			UISize New;
			New.Mode = SizeMode::ScreenRelative;
			New.Value = sr;
			return New;
		}

		static UISize Parent(float Parent)
		{
			UISize New;
			New.Mode = SizeMode::ParentRelative;
			New.Value = Parent;
			return New;
		}
		Vec2f GetPixels(Window* With = GetActiveWindow()) const;
		Vec2f GetScreen(Window* With = GetActiveWindow()) const;

		bool IsParentRelative() const { return Mode == SizeMode::ParentRelative; }

		static UISize Smallest()
		{
			UISize New;
			New.Mode = SizeMode::ScreenRelative;
			New.Value = 0;
			return New;
		}
		static UISize Largest()
		{
			UISize New;
			New.Mode = SizeMode::ScreenRelative;
			New.Value = INFINITY;
			return New;
		}
	};

	struct SizeVec
	{
		UISize X;
		UISize Y;

		SizeVec(UISize X, UISize Y);
		SizeVec(UISize XY);
		SizeVec(Vec2f XY, SizeMode Mode);
		SizeVec(Vec2f XY);
		SizeVec(float XY);
		SizeVec();

		static SizeVec Pixels(float X, float Y);

		Vec2f GetPixels(Window* With = UISize::GetActiveWindow()) const;
		Vec2f GetScreen(Window* With = UISize::GetActiveWindow()) const;

		static SizeVec Smallest()
		{
			return SizeVec(0, SizeMode::ScreenRelative);
		}
		static SizeVec Largest()
		{
			return SizeVec(INFINITY, SizeMode::ScreenRelative);
		}

		bool operator==(const SizeVec& other) const;
	};
}

kui::UISize operator ""_px(long double);
kui::UISize operator ""_px(unsigned long long int);