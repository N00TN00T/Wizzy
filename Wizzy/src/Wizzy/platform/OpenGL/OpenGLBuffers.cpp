#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/platform/OpenGL/OpenGLBuffers.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"

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
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW));
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
}
