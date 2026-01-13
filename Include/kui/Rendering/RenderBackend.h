#pragma once
#include <kui/UI/UIBackground.h>
#include <kui/Font.h>
#include "RedrawArea.h"

namespace kui
{
	class Window;
	class FontRenderData;
	class WindowColors;
}

namespace kui::render
{
	class RenderBackend
	{
	public:

		virtual ~RenderBackend() = default;
		virtual void Initialize(Window* From) = 0;
		virtual void CreateBuffer(Vec2ui Size) = 0;
		virtual UIBackgroundState* MakeBackground() = 0;
		virtual FontRenderData* MakeFont(uint8_t* ImageData, uint32_t Width, uint32_t Height, float CharSize) = 0;
		virtual DrawableText* MakeText(FontRenderData* Data, const std::vector<Font::RenderGlyph>& Glyphs,
			float Scale, Vec3f Color, float Opacity) = 0;

		virtual void DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color, unsigned int Texture) = 0;
		virtual void BeginFrame(Window* Target) = 0;
		virtual void EndFrame(Window* Target) = 0;

		virtual void BeginArea(Window* Target, RedrawBox Box) = 0;

		virtual void DrawToWindow(Window* Target) = 0;

		virtual unsigned int CreateTexture(uint8_t* Bytes, std::size_t Width, std::size_t Height) = 0;
		virtual void FreeTexture(unsigned int Target) = 0;
	};
}