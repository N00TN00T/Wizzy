#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/GLBuffers.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy
{
    /*
        ***************************************************'
                        VERTEX BUFFER IMPL
        ***************************************************'
    */
    GLVertexBuffer::GLVertexBuffer(void* data, size_t size, BufferEnum usage)
    {
        WZ_CORE_TRACE("Creating GL vertex buffer");
        GL_CALL(glGenBuffers(1, &m_id));
        this->Bind();
        auto t = BufferUsageToAPIBufferUsage(usage);
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, t));
        WZ_CORE_INFO("Successfully created GL vertex buffer with id '{0}'", m_id);
        this->Unbind();
    }
    GLVertexBuffer::~GLVertexBuffer()
    {
        WZ_CORE_TRACE("Destructing GL vertex buffer with id '{0}'", m_id);
        GL_CALL(glDeleteBuffers(1, &m_id));
    }

    void GLVertexBuffer::SetData(void* data, size_t size)
    {
        this->Bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
        this->Unbind();
    }

    void GLVertexBuffer::Bind() const
    {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
    }
    void GLVertexBuffer::Unbind() const
    {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void* GLVertexBuffer::Map()
    {
        this->Bind();

        GL_CALL(auto* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

        return p;
    }

    void GLVertexBuffer::Unmap()
    {
        this->Bind();
        GL_CALL(glUnmapBuffer(GL_ARRAY_BUFFER));
    }

    /*
        ***************************************************'
                        INDEX BUFFER IMPL
        ***************************************************'
    */
    GLIndexBuffer::GLIndexBuffer(u32* data, u32 count)
        : m_count(count)
    {
        WZ_CORE_TRACE("Creating GL index buffer");
        GL_CALL(glGenBuffers(1, &m_id));
        this->Bind();
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW));
        this->Unbind();
        WZ_CORE_INFO("Successfully created GL index buffer with id '{0}'", m_id);
    }
    GLIndexBuffer::~GLIndexBuffer()
    {
        WZ_CORE_TRACE("Destructing GL index buffer with id '{0}'", m_id);
        GL_CALL(glDeleteBuffers(1, &m_id));
    }

    void GLIndexBuffer::SetData(u32* data, u32 count)
    {
        this->Bind();
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW));
        this->Unbind();
    }

    void GLIndexBuffer::Bind() const
    {
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
    }
    void GLIndexBuffer::Unbind() const
    {
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    /*
        ***************************************************'
                        VERTEX ARRAY IMPL
        ***************************************************'
    */
    GLVertexArray::GLVertexArray()
    {
        WZ_CORE_TRACE("Creating GL vertex array");
        GL_CALL(glGenVertexArrays(1, &m_id));
        this->Unbind();
        WZ_CORE_INFO("Successfully created GL vertex array with id '{0}'", m_id);
    }
    GLVertexArray::~GLVertexArray()
    {
        GL_CALL(glDeleteVertexArrays(1, &m_id));
    }

    void GLVertexArray::Bind() const
    {
        GL_CALL(glBindVertexArray(m_id));
    }
    void GLVertexArray::Unbind() const
    {
        GL_CALL(glBindVertexArray(0));
    }

    void GLVertexArray::PushVertexBuffer(const VertexBufferPtr& buffer)
    {
        this->Bind();
        buffer->Bind();

        const auto& _layout = buffer->GetLayout();
        WZ_CORE_ASSERT(_layout.GetElements().size() != 0, "Vertex buffer has invalid layout");
        for (const auto& _element : _layout.GetElements())
        {
            GL_CALL(glEnableVertexAttribArray(m_vertexBufferIndex));
            GL_CALL(glVertexAttribPointer(
                m_vertexBufferIndex,
                _element.GetComponentCount(),
                ShaderDataTypeToAPIType(_element.type),
                _element.normalized ? GL_TRUE : GL_FALSE,
                _layout.GetStride(),
                reinterpret_cast<const void*>(_element.offset)
            ));
            m_vertexBufferIndex++;
        }

        this->Unbind();
        buffer->Unbind();

        m_vertexBuffers.push_back(buffer);
    }
    void GLVertexArray::SetIndexBuffer(const IndexBufferPtr& buffer)
    {
        this->Bind();
        buffer->Bind();

        this->Unbind();
        buffer->Unbind();

        m_indexBuffer = buffer;
    }
}