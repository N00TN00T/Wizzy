#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Graphics/VertexBuffer.h"
#include "Wizzy/Graphics/GLErrorHandling.h"

namespace Wizzy {

	VertexBuffer::VertexBuffer(const void * data, size_bytes size) {
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_CALL(glGenBuffers(1, &m_bufferId));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferId));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	VertexBuffer::~VertexBuffer() {
		GL_CALL(glDeleteBuffers(1, &m_bufferId));
		WZ_CORE_TRACE("Destructed a vertex buffer with id {0}", m_bufferId);
	}
	void VertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
	}
}