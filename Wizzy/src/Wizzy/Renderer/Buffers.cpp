#include "wzpch.h"

#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/Renderer.h"

#include "Wizzy/platform/OpenGL/GLBuffers.h"

namespace Wizzy {
    VertexBuffer* VertexBuffer::Create(void *data, size_t size, BufferEnum usage) {

        CREATE_BY_API(new GLVertexBuffer(data, size, usage));

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(u32 *data, u32 count) {

        CREATE_BY_API(new GLIndexBuffer(data, count));

        return nullptr;
    }

    VertexArray* VertexArray::Create() {

        CREATE_BY_API(new GLVertexArray());

        return nullptr;
    }
}
