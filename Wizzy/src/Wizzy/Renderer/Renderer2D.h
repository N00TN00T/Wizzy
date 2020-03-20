#pragma once

#include "Wizzy/Rect.h"
#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/Font.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/Shader.h"

namespace Wizzy
{
    enum class RectMode
    {
        Filled,
        Outlined
    };

    class Renderer2D
    {
    private:
        struct VertexData
        {
            vec2 pos;
            vec2 uv;
            Color color;
            float textureSlot;
        };
        // NOTE: The indices will take ~20% more of the memory budget
        static constexpr size_t VERTEX_SIZE             = (sizeof(VertexData));
        static constexpr size_t MEMORY_BUDGET           = (_MB(150)); // + 20%
        static constexpr size_t OBJECT_SIZE             = (VERTEX_SIZE * 4);
        static constexpr size_t MAX_OBJECTS             = (MEMORY_BUDGET / OBJECT_SIZE);
        static constexpr size_t MAX_INDICES             = (MAX_OBJECTS * 6);
        static size_t TEXTURE_SLOTS;
    public:

        static void Begin(Shader::Handle hShader, const mat4& cameraTransform = mat4(1), RenderTarget::Handle renderTarget = NULL);

        static void SubmitImage(Texture::Handle hTexture,  const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);
        static void SubmitImage(Texture::Handle hTexture, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

        static void SubmitImage(RenderTarget::Handle hTexture, const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);
        static void SubmitImage(RenderTarget::Handle hTexture, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

        static void SubmitText(const string& text, Font* font, const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);
        static void SubmitText(const string& text, Font* font, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

        static void SubmitRect(const Rect& rect, const glm::vec4& color, RectMode mode = RectMode::Filled, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);
        
        static void End(RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

        static bool IsReady(RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

    private:
        static void Init(RenderTarget::Handle hRenderTarget);

        static void Submit(u32 slot, const vec2& size, mat4 transform, const glm::vec4& color, RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE);

    private:
        struct RenderTargetData
        {
            bool ready = false;
            VertexBufferPtr vbo;
            VertexArrayPtr vao;
            bool initialized = false;
            VertexData* pBufferData;
            std::unordered_map<u32, u32> textureSlots;
            u32 slotCount = 0;
            Shader::Handle hShader;
            u32 indexCount = 0;
            u32 submissionCount = 0;
            std::mutex rendererMutex;
            Texture::Handle hWhiteTexture;
            mat4 camTransform;
        };
        
        static RenderTargetData s_windowTargetData;
        static std::unordered_map<RenderTarget::Handle, RenderTargetData, Resource::Handle::hash> s_renderTargetData;
    };
}