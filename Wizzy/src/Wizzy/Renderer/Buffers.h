#pragma once

namespace Wizzy {

    enum class ShaderDataType {
        NONE = 0,
        FLOAT1, FLOAT2, FLOAT3, FLOAT4,
        MAT3, MAT4,
        INT1, INT2, INT3, INT4,
        BOOL
    };

    inline
    size_t ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::NONE:      break;
            case ShaderDataType::FLOAT1:    return 1 * 4;
            case ShaderDataType::FLOAT2:    return 2 * 4;
            case ShaderDataType::FLOAT3:    return 3 * 4;
            case ShaderDataType::FLOAT4:    return 4 * 4;
            case ShaderDataType::MAT3:      return 9 * 4;
            case ShaderDataType::MAT4:      return 16 * 4;
            case ShaderDataType::INT1:      return 1 * 4;
            case ShaderDataType::INT2:      return 2 * 4;
            case ShaderDataType::INT3:      return 3 * 4;
            case ShaderDataType::INT4:      return 4 * 4;
            case ShaderDataType::BOOL:      return 1;
        }

        WZ_CORE_ASSERT(false, "Unimplemented shader data type");
        return 0;
    }

    inline
    u32 ShaderDataTypeElementCount(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::NONE:      break;
            case ShaderDataType::FLOAT1:    return 1;
            case ShaderDataType::FLOAT2:    return 2;
            case ShaderDataType::FLOAT3:    return 3;
            case ShaderDataType::FLOAT4:    return 4;
            case ShaderDataType::MAT3:      return 9;
            case ShaderDataType::MAT4:      return 16;
            case ShaderDataType::INT1:      return 1;
            case ShaderDataType::INT2:      return 2;
            case ShaderDataType::INT3:      return 3;
            case ShaderDataType::INT4:      return 4;
            case ShaderDataType::BOOL:      return 1;
        }

        WZ_CORE_ASSERT(false, "Unimplemented shader data type");
        return 0;
    }

    struct BufferElement {
        ShaderDataType type;
        string name;
        u32 size;
        u32 offset;
        bool normalized;

        BufferElement() {}
        BufferElement(ShaderDataType type, const string& name,
                        bool normalized = false)
            :  type(type), name(name),
                size(ShaderDataTypeSize(type)), offset(0),
                normalized(normalized) {}

        inline
        u32 GetComponentCount() const { return ShaderDataTypeElementCount(type); }
    };

    class BufferLayout {
    public:

        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_elements(elements) {
            u32 _offset = 0;
            m_stride = 0;
            for (auto& _element : m_elements) {
                _element.offset = _offset;
                _offset += _element.size;
                m_stride += _element.size;
            }
        }

        inline
        const std::vector<BufferElement>& GetElements() const { return m_elements; }
        inline u32 GetStride() const { return m_stride; }

    private:
        std::vector<BufferElement> m_elements;
        u32 m_stride = 0;
    };

    class VertexBuffer {
    public:
        virtual
        ~VertexBuffer() {}

        virtual
        void SetData(void *data, size_t size) = 0;

        virtual
        void Bind() const = 0;
        virtual
        void Unbind() const = 0;

        virtual
        void SetLayout(const BufferLayout& layout) = 0;
        virtual
        const BufferLayout& GetLayout() const = 0;

        static
        VertexBuffer* Create(void *data, size_t size);
    };

    class IndexBuffer {
    public:
        virtual
        ~IndexBuffer() {}

        virtual
        void SetData(u32 *data, u32 count) = 0;

        virtual
        void Bind() const = 0;
        virtual
        void Unbind() const = 0;

        virtual
        u32 GetCount() const = 0;

        static
        IndexBuffer* Create(u32 *data, u32 count);
    };

    typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
    typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

    class VertexArray {
    public:
        virtual
        ~VertexArray() {}

        virtual
        void Bind() const = 0;
        virtual
        void Unbind() const = 0;

        virtual
        void PushVertexBuffer(const VertexBufferPtr& buffer) = 0;
        virtual
        void SetIndexBuffer(const IndexBufferPtr& buffer) = 0;

        virtual
        const std::vector<VertexBufferPtr>& GetVertexBuffers() const = 0;
        virtual
        const IndexBufferPtr& GetIndexBuffer() const = 0;

        static
        VertexArray* Create();
    };

    typedef std::shared_ptr<VertexArray> VertexArrayPtr;

}
