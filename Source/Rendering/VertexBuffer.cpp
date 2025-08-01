#include "VertexBuffer.h"
#include "OpenGL.h"
#include <iostream>
using namespace kui::render;

GLVertexBuffer::GLVertexBuffer(std::vector<GLVertex> Vertices, std::vector<unsigned int> Indices)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLVertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, CornerIndex));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	IndicesSize = static_cast<unsigned int>(Indices.size());
}

GLVertexBuffer::~GLVertexBuffer()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void GLVertexBuffer::Bind()
{
	glBindVertexArray(VAO);
}

void GLVertexBuffer::Unbind()
{
	glBindVertexArray(0);
}

void GLVertexBuffer::Draw()
{
	Bind();
	glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, 0);
	Unbind();
}
