#pragma once

namespace Wizzy {
	struct VertexBufferElement {
		u32 type;
		u32 count;
		size_t size;
		bool normalized;
	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout();
		~VertexBufferLayout();

		template <typename T>
		inline void Push(u32 count) {
			WZ_CORE_ASSERT(false, "Type '" + string(typeid(T).name()) + "' is not a supported buffer layout type");
		}

		template <>
		inline void Push<float>(u32 count) {
			PushFloat(count);
		}

		template <>
		inline void Push<u32>(u32 count) {
			PushU32(count);
		}

		template <>
		inline void Push<u16>(u32 count) {
			PushU16(count);
		}

		template <>
		inline void Push<u8>(u32 count) {
			PushU8(count);
		}

		inline const std::vector<VertexBufferElement>& GetElements() const {
			return m_elements;
		}

		inline const u32& GetStride() const {
			return m_stride;
		}

	private:
		void PushFloat(u32 count);
		void PushU32(u32 count);
		void PushU16(u32 count);
		void PushU8(u32 count);

	private:
		std::vector<VertexBufferElement> m_elements;
		u32 m_stride = 0;
	};
}