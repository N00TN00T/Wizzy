#pragma once

#include "Wizzy/Renderer/Buffers.h"

namespace Wizzy {
    class GLVertexBuffer
        : public VertexBuffer {
    public:
        GLVertexBuffer(void *data, size_t size);
        virtual
        ~GLVertexBuffer();

        virtual
        void SetData(void *data, size_t size) override;

        virtual
        void Bind() const override;
        virtual
        void Unbind() const override;

        inline virtual
        void SetLayout(const BufferLayout& layout) override { m_layout = layout; }
        inline virtual
        const BufferLayout& GetLayout() const override { return m_layout; }

    private:
        u32 m_id;
        BufferLayout m_layout;
    };

    class GLIndexBuffer
        : public IndexBuffer {
    public:
        GLIndexBuffer(u32 *data, u32 count);
        virtual
        ~GLIndexBuffer();

        virtual
        void SetData(u32 *data, u32 count) override;

        inline virtual
        u32 GetCount() const override { return m_count; }

        virtual
        void Bind() const override;
        virtual
        void Unbind() const override;

    private:
        u32 m_id;
        u32 m_count;
    };



    class GLVertexArray
        : public VertexArray{
    public:
        GLVertexArray();
        virtual
        ~GLVertexArray();

        virtual
        void Bind() const override;
        virtual
        void Unbind() const override;

        virtual
        void PushVertexBuffer(const VertexBufferPtr& buffer) override;
        virtual
        void SetIndexBuffer(const IndexBufferPtr& buffer) override;

        virtual inline
        const std::vector<VertexBufferPtr>& GetVertexBuffers() const override {
            return m_vertexBuffers;
        }
        virtual inline
        const IndexBufferPtr& GetIndexBuffer() const override {
            return m_indexBuffer;
        }

    private:
        u32 m_id;
        IndexBufferPtr m_indexBuffer;
        std::vector<VertexBufferPtr> m_vertexBuffers;
    };
}
