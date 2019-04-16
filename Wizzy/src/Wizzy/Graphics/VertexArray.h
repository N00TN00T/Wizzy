#pragma once

#include "Wizzy/Graphics/VertexBuffer.h";
#include "Wizzy/Graphics//VertexBufferLayout.h"

namespace Wizzy {

	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout);

		void Bind() const;

	private:
		u32 m_vaoId;
	};
}