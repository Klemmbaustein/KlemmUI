#pragma once
#include "RenderBackend.h"

namespace kui::render
{
	class GLVertexBuffer;

	class OpenGLBackend : public RenderBackend
	{
	public:
		OpenGLBackend();
		~OpenGLBackend();

		void Initialize(Window* From) override;

		void CreateBuffer(Vec2ui Size) override;

		UIBackgroundState* MakeBackground() override;
		void BeginFrame(Window* Target) override;
		void EndFrame(Window* Target) override;
		void DrawSimpleBox(Vec2f Position, Vec2f Size, Vec3f Color, unsigned int Texture) override;

		void BeginArea(Window* Target, RedrawBox Box) override;
		void DrawToWindow(Window* Target) override;
		FontRenderData* MakeFont(uint8_t* ImageData, uint32_t Width, uint32_t Height, float CharSize) override;
		DrawableText* MakeText(FontRenderData* Data, const std::vector<Font::RenderGlyph>& Glyphs,
			float Scale, Vec3f Color, float Opacity) override;
		unsigned int CreateTexture(uint8_t* Bytes, std::size_t Width, std::size_t Height) override;
		void FreeTexture(unsigned int Target) override;

		friend class GLUIBackgroundState;
		friend class GLDrawableText;

		bool CanDrawToWindow = true;
		bool FlipImage = false;
		static bool UseAlphaBuffer;
		unsigned int TargetBuffer = 0;
		unsigned int UIBuffer = 0;
		unsigned int UITextures[2];
		GLVertexBuffer* BoxVertexBuffer = nullptr;
		Shader* BackgroundShader = nullptr;
		Shader* TextShader = nullptr;
		Shader* WindowShader = nullptr;
		void UpdateScroll(ScrollObject* Scroll, Shader* UsedShader, UIBackgroundState* Target);

	private:
		Vec2ui ScissorXY, ScissorWH;
	};

	class GLFontRenderData : public kui::FontRenderData
	{
	public:
		GLFontRenderData() = default;
		~GLFontRenderData();

		unsigned int FontTexture = 0;
		unsigned int FontVertexBufferId = 0;
		FontVertex* FontVertexBufferData = 0;
		uint32_t FontVertexBufferCapacity = 0;
	};

	class GLDrawableText : public DrawableText
	{
		unsigned int VAO = 0, VBO = 0;
		unsigned int Texture = 0;
		unsigned int NumVerts = 0;
		OpenGLBackend* Parent = nullptr;
	public:
		GLDrawableText(OpenGLBackend* Parent, unsigned int VAO, unsigned int VBO,
			unsigned int NumVerts, unsigned int Texture, float Scale, Vec3f Color,
			float Opacity);
		~GLDrawableText();

		void Draw(ScrollObject* CurrentScrollObject, Vec2f Pos) override;
	};

	class GLUIBackgroundState : public UIBackgroundState
	{
	public:
		GLUIBackgroundState(Shader* UsedShader)
		{
			this->UsedShader = UsedShader;
		}

		Shader* UsedShader = nullptr;
		void Draw(render::RenderBackend* With, Vec2f Position, Vec2f Size,
			ScrollObject* Scroll, WindowColors* Colors) override;
	};
}
