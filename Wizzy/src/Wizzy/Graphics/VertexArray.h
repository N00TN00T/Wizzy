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
		void Unbind() const;

		inline const u32& GetId() const { return m_vaoId; }

	private:
		u32 m_vaoId;
	};
}
