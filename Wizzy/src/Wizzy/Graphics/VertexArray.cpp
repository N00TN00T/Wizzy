#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Graphics/VertexArray.h"
#include "Wizzy/Graphics/GLErrorHandling.h"

namespace Wizzy {

	VertexArray::VertexArray() {
		GL_CALL(glGenVertexArrays(1, &m_vaoId));
	}
	VertexArray::~VertexArray() {
		GL_CALL(glDeleteVertexArrays(1, &m_vaoId));
		WZ_CORE_TRACE("Desctructed vertex array with id {0}", m_vaoId);
	}
	void VertexArray::AddBuffer(const VertexBuffer & vbo, const VertexBufferLayout & layout) {
		Bind();
		vbo.Bind();
		const auto& _elements = layout.GetElements();
		u32 _offset = 0;
		for (u32 i = 0; i < _elements.size(); i++) {
			const auto& _element = _elements[i];

			GL_CALL(glEnableVertexAttribArray(i));
			GL_CALL(glVertexAttribPointer(i, _element.count, _element.type, _element.normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)_offset));
			_offset += _element.size;
		}
		GL_CALL();
	}
	
	void VertexArray::Bind() const {
		GL_CALL(glBindVertexArray(m_vaoId));
	}
}