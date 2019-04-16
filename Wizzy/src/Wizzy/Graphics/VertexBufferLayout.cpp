#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Graphics/VertexBufferLayout.h"

namespace Wizzy {
	VertexBufferLayout::VertexBufferLayout() {
	}
	VertexBufferLayout::~VertexBufferLayout() {
	}
	void VertexBufferLayout::PushFloat(u32 count) {
		size_t _size = count * sizeof(float);
		m_elements.push_back({ GL_FLOAT, count, _size, false });
		m_stride += _size;
	}
	void VertexBufferLayout::PushU32(u32 count) {
		size_t _size = count * sizeof(u32);
		m_elements.push_back({ GL_UNSIGNED_INT, count, _size, false });
		m_stride += _size;
	}
	void VertexBufferLayout::PushU16(u32 count) {
		size_t _size = count * sizeof(u16);
		m_elements.push_back({ GL_UNSIGNED_SHORT, count, _size, false });
		m_stride += _size;
	}
	void VertexBufferLayout::PushU8(u32 count) {
		size_t _size = count * sizeof(u8);
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, _size, false });
		m_stride += _size;
	}
}