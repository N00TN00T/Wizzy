#pragma once

namespace Wizzy {
	class VertexBuffer {
	public:
		VertexBuffer(const void *data, size_bytes size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		inline const u32& GetId() const { return m_bufferId; }

	private:
		u32			m_bufferId;
	};
}
