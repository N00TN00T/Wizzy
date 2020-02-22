#pragma once

#include "Wizzy/Renderer/API.h"

namespace Wizzy {

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

        virtual void* Map() = 0;
        virtual void Unmap() = 0;

        static
        VertexBuffer* Create(void *data, size_t size, BufferEnum usage = WZ_BUFFER_USAGE_STATIC_DRAW);
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
