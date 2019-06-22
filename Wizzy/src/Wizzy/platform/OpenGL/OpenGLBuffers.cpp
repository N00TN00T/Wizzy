#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/OpenGLBuffers.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

namespace Wizzy {
    /*
        ***************************************************'
                        VERTEX BUFFER IMPL
        ***************************************************'
    */
    GLVertexBuffer::GLVertexBuffer(void *data, size_t size) {
        GL_CALL(glCreateBuffers(1, &m_id));
        this->Bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }
    GLVertexBuffer::~GLVertexBuffer() {
        GL_CALL(glDeleteBuffers(1, &m_id));
    }

    void GLVertexBuffer::SetData(void *data, size_t size) {
        this->Bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }

    void GLVertexBuffer::Bind() const {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
    }
    void GLVertexBuffer::Unbind() const {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    /*
        ***************************************************'
                        INDEX BUFFER IMPL
        ***************************************************'
    */
    GLIndexBuffer::GLIndexBuffer(u32 *data, u32 count)
        : m_count(count) {
        GL_CALL(glCreateBuffers(1, &m_id));
        this->Bind();
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW));
    }
    GLIndexBuffer::~GLIndexBuffer() {
        GL_CALL(glDeleteBuffers(1, &m_id));
    }

    void GLIndexBuffer::SetData(u32 *data, u32 count) {
        this->Bind();
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW));
    }

    void GLIndexBuffer::Bind() const {
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
    }
    void GLIndexBuffer::Unbind() const {
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    /*
        ***************************************************'
                        VERTEX ARRAY IMPL
        ***************************************************'
    */
    GLVertexArray::GLVertexArray() {
        GL_CALL(glCreateVertexArrays(1, &m_id));
        this->Unbind();
    }
    GLVertexArray::~GLVertexArray() {
        GL_CALL(glDeleteVertexArrays(1, &m_id));
    }

    void GLVertexArray::Bind() const  {
        GL_CALL(glBindVertexArray(m_id));
    }
    void GLVertexArray::Unbind() const {
        GL_CALL(glBindVertexArray(0));
    }

    void GLVertexArray::PushVertexBuffer(const VertexBufferPtr& buffer) {
        this->Bind();
        buffer->Bind();

        u32 _index = 0;
        const auto& _layout = buffer->GetLayout();
        WZ_CORE_ASSERT(_layout.GetElements().size() != 0, "Vertex buffer has invalid layout");
        for (const auto& _element : _layout.GetElements()) {
            GL_CALL(glEnableVertexAttribArray(_index));
            GL_CALL(glVertexAttribPointer(
                _index,
                _element.GetComponentCount(),
                GLShaderDataTypeToAPIType(_element.type),
                _element.normalized ? GL_TRUE : GL_FALSE,
                _layout.GetStride(),
                reinterpret_cast<const void*>(_element.offset)
            ));
            _index++;
        }

        //this->Unbind();
        //buffer->Unbind();

        m_vertexBuffers.push_back(buffer);
    }
    void GLVertexArray::SetIndexBuffer(const IndexBufferPtr& buffer) {
        this->Bind();
        buffer->Bind();

        //this->Unbind();
        //buffer->Unbind();

        m_indexBuffer = buffer;
    }
}
