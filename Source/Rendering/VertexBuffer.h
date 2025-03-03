#pragma once
#include <vector>
#include <kui/Vec2.h>

namespace kui
{
	struct Vertex
	{
		Vec2f Position;
		float CornerIndex = 0;
	};

	struct VertexBuffer
	{
		unsigned int VAO = 0u, VBO = 0u, EBO = 0u, IndicesSize = 0u;
		std::vector<Vertex> Vertices; std::vector<unsigned int> Indices;
	public:

		VertexBuffer(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices);
		~VertexBuffer();
		void Bind();
		void Unbind();

		void Draw();
	};
}