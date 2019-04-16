#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Graphics/IndexBuffer.h"
#include "Wizzy/Graphics/GLErrorHandling.h"

namespace Wizzy {
	IndexBuffer::IndexBuffer(const u32 *indices, u32 count)
		: m_count(count) {
		WZ_CORE_ASSERT(sizeof(u32) == sizeof(GLuint), "This platform is not supported");

		GL_CALL(glGenBuffers(1, &m_bufferId));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW));
	}
	IndexBuffer::~IndexBuffer() {
		GL_CALL(glDeleteBuffers(1, &m_bufferId));
		WZ_CORE_TRACE("Destructed an index buffer with id {0}", m_bufferId);
	}
	void IndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
	}
}