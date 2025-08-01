#pragma once
#include "UIBackground.h"
#include <set>

namespace kui
{
	/**
	 * @brief
	 * A half transparent background that displays everything behind it using a blur effect.
	 *
	 * It functions like a regular UIBackground otherwise
	 */
	class UIBlurBackground : public UIBackground
	{
	protected:
		Vec2ui GetPixelSize();
		bool BuffersLoaded = false;
		unsigned int BackgroundBuffers[2], BackgroundTextures[2];
		Shader* BlurShader = nullptr;
		Vec2ui OldSize;
		void CreateBlurBuffers();
		static thread_local std::set<UIBlurBackground*> BlurBackgrounds;
		friend class UIManager;
	public:
		UIBlurBackground(bool Horizontal, Vec2f Position, Vec3f Color, float Opacity = 0.5f, SizeVec MinScale = SizeVec::Smallest());
		virtual ~UIBlurBackground() override;

		void Draw(render::RenderBackend* Backend) override;

	};
}