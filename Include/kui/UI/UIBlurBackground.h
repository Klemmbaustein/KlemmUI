#pragma once
#include "UIBackground.h"
#include <set>

namespace kui
{
	class UIBlurBackground : public UIBackground
	{
	protected:
		Vec2ui GetPixelSize();
		unsigned int BackgroundBuffer = 0, BackgroundTexture = 0;
		Shader* BlurShader = nullptr;
		Vec2ui OldSize;
		void CreateBlurBuffer();
		static std::set<UIBlurBackground*> BlurBackgrounds;
		friend class UIManager;
	public:
		UIBlurBackground(bool Horizontal, Vec2f Position, Vec3f Color, float Opacity = 0.5f, Vec2f MinScale = Vec2f(0));
		virtual ~UIBlurBackground() override;

		void Draw() override;

	};
}