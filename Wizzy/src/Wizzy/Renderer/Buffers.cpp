#include "wzpch.h"

#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/Renderer.h"

#include "Wizzy/platform/OpenGL/OpenGLBuffers.h"

namespace Wizzy {
    VertexBuffer* VertexBuffer::Create(void *data, size_t size) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API_NONE: WZ_CORE_ERROR("No renderer API is selected"); return nullptr;
            case RendererAPI::API_OPENGL: return new GLVertexBuffer(data, size); break;
        }

        WZ_CORE_ASSERT(false, "Invalid renderer API selected");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(u32 *data, u32 count) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API_NONE: WZ_CORE_ERROR("No renderer API is selected"); return nullptr;
            case RendererAPI::API_OPENGL: return new GLIndexBuffer(data, count); break;
        }

        WZ_CORE_ASSERT(false, "Invalid renderer API selected");
        return nullptr;
    }

    VertexArray* VertexArray::Create() {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API_NONE: WZ_CORE_ERROR("No renderer API is selected"); return nullptr;
            case RendererAPI::API_OPENGL: return new GLVertexArray(); break;
        }

        WZ_CORE_ASSERT(false, "Invalid renderer API selected");
        return nullptr;
    }
}
