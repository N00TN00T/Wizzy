#pragma once

#include "Renderer.h"
#include "Wizzy/Rect.h"
#include "Wizzy/Renderer/Texture.h"

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

        static void Begin(const Shader::Handle& hndShader, const mat4& cameraTransform = mat4(1), RenderTargetPtr renderTarget = NULL);

        static void SubmitImage(Texture::Handle texture,  const glm::vec2& position, const glm::vec2 scale, float rotation, const glm::vec4& color, RenderTargetPtr renderTarget = NULL);
        static void SubmitImage(Texture::Handle texture,  mat4 transform, const glm::vec4& color, RenderTargetPtr renderTarget = NULL);

        //static void SubmitText(Font::Handle font, const string& text, const glm::vec2& position, const glm::vec2 scale, const glm::vec4& color);

        static void SubmitRect(const Rect& rect, const glm::vec4& color, RectMode mode = RectMode::Filled, RenderTargetPtr renderTarget = NULL);
        
        static void End(RenderTargetPtr renderTarget = NULL);

        inline static bool IsReady(RenderTargetPtr renderTarget = NULL) 
        {
            RenderTargetData& tData = renderTarget ? s_renderTargetData[renderTarget] : s_windowTargetData;
            return tData.ready; 
        }

    private:
        static void Init(RenderTargetPtr renderTarget);

    

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
            Shader::Handle shader;
            u32 indexCount = 0;
            u32 submissionCount = 0;
            std::mutex rendererMutex;
            Texture::Handle hndWhiteTexture = WZ_NULL_RESOURCE_HANDLE;
            mat4 camTransform;
        };
        
        static RenderTargetData s_windowTargetData;
        static std::unordered_map<RenderTargetPtr, RenderTargetData> s_renderTargetData;
    };
}