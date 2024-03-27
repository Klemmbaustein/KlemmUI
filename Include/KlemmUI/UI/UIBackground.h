#pragma once
#include "UIBox.h"
#include "../Vector3.h"
#include "../Vector2.h"

namespace KlemmUI
{
	class Shader;
	struct VertexBuffer;

	class UIBackground : public UIBox
	{
		void ScrollTick(Shader* UsedShader);
		void MakeGLBuffers(bool InvertTextureCoordinates = false);
		bool UseTexture = false;
		unsigned int TextureID = 0;
	protected:
		Shader* BackgroundShader;
		virtual void DrawBackground();
		Vector3f Color;
		VertexBuffer* BoxVertexBuffer = nullptr;
		float Opacity = 1;
		Vector3f ColorMultiplier = 1;
	public:
		UIBackground* SetOpacity(float NewOpacity);
		float GetOpacity();
		void SetColor(Vector3f NewColor);
		Vector3f GetColor();
		void SetInvertTextureCoordinates(bool Invert);
		UIBackground* SetUseTexture(bool UseTexture, unsigned int TextureID = 0);
		UIBackground(bool Horizontal, Vector2f Position, Vector3f Color, Vector2f MinScale = Vector2f(0), Shader* UsedShader = nullptr);
		virtual ~UIBackground();
		void Draw() override;
		void Update() override;
		void OnAttached() override;
	};
}