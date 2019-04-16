#pragma once

namespace Wizzy {
	class VertexBuffer {
	public:
		VertexBuffer(const void *data, size_bytes size);
		~VertexBuffer();

		void Bind() const;
	private:
		u32			m_bufferId;
	};
}