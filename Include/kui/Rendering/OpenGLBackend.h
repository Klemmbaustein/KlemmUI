#pragma once
#include "RenderBackend.h"

namespace kui::render
{
	class GLVertexBuffer;

	class OpenGLBackend : public RenderBackend
	{
	public:
		OpenGLBackend(Window* From);
		~OpenGLBackend();

		void CreateBuffer(Vec2ui Size) override;

		UIBackgroundState* MakeBackground() override;
		void BeginFrame(Window* Target) override;
		void EndFrame(Window* Target) override;
		void DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color) override;

		void BeginArea(Window* Target, RedrawBox Box) override;
		void DrawToWindow(Window* Target) override;

		friend class GLUIBackgroundState;

		bool CanDrawToWindow = true;
		bool UseAlphaBuffer = false;
		unsigned int TargetBuffer = 0;
		unsigned int UIBuffer = 0;
		unsigned int UITextures[2];

	private:
		Vec2ui ScissorXY, ScissorWH;
		GLVertexBuffer* BoxVertexBuffer;
		Shader* BackgroundShader = nullptr;
		void UpdateScroll(ScrollObject* Scroll, Shader* UsedShader, UIBackgroundState* Target);
	};

	class GLUIBackgroundState : public UIBackgroundState
	{
	public:
		GLUIBackgroundState(Shader* UsedShader)
		{
			this->UsedShader = UsedShader;
		}

		Shader* UsedShader = nullptr;
		void Draw(render::RenderBackend* With, Vec2f Position, Vec2f Size, ScrollObject* Scroll) override;
	};
}
