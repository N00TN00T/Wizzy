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
			if (typeid(T) == typeid(float)) 	PushFloat(count);
			else if (typeid(T) == typeid(u32)) 	PushU32(count);
			else if (typeid(T) == typeid(u16)) 	PushU16(count);
			else if (typeid(T) == typeid(u8)) 	PushU8(count);
			else WZ_CORE_ASSERT(false, R"(Tried pushing an unsupported type to
										a buffer layout)");
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
