#pragma once
#include <vector>
#include <kui/Vec2.h>

namespace kui::render
{
	struct GLVertex
	{
		Vec2f Position;
		float CornerIndex = 0;
	};

	struct GLVertexBuffer
	{
		unsigned int VAO = 0u, VBO = 0u, EBO = 0u, IndicesSize = 0u;
	public:

		GLVertexBuffer(std::vector<GLVertex> Vertices, std::vector<unsigned int> Indices);
		~GLVertexBuffer();
		void Bind();
		void Unbind();

		void Draw();
	};
}