#pragma once
#include <kui/UI/UIBackground.h>
#include "RedrawArea.h"

namespace kui
{
	class Window;
}

namespace kui::render
{
	class RenderBackend
	{
	public:
		virtual ~RenderBackend() = default;
		virtual void CreateBuffer(Vec2ui Size) = 0;
		virtual UIBackgroundState* MakeBackground() = 0;
		virtual void DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color) = 0;
		virtual void BeginFrame(Window* Target) = 0;
		virtual void EndFrame(Window* Target) = 0;

		virtual void BeginArea(Window* Target, RedrawBox Box) = 0;

		virtual void DrawToWindow(Window* Target) = 0;
	};
}