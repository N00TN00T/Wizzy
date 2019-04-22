#pragma once

namespace Wizzy {
	class IndexBuffer {
	public:
		IndexBuffer(const u32 *indices, u32 count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;
		inline u32 GetCount() const { return m_count; }
	private:
		u32			m_bufferId;
		u32			m_count;
	};
}
