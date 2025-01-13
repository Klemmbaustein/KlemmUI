#pragma once
#include "UIBackground.h"
#include <set>

namespace kui
{
	class UIBlurBackground : public UIBackground
	{
	protected:
		Vec2ui GetPixelSize();
		bool BuffersLoaded = false;
		unsigned int BackgroundBuffers[2], BackgroundTextures[2];
		Shader* BlurShader = nullptr;
		Vec2ui OldSize;
		void CreateBlurBuffers();
		static std::set<UIBlurBackground*> BlurBackgrounds;
		friend class UIManager;
	public:
		UIBlurBackground(bool Horizontal, Vec2f Position, Vec3f Color, float Opacity = 0.5f, SizeVec MinScale = SizeVec::Smallest());
		virtual ~UIBlurBackground() override;

		void Draw() override;

	};
}