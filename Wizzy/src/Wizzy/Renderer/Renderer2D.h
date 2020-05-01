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
    public:
        typedef u32 PipelineId;

        struct VertexData
        {
            vec2    pos;
            vec2    uv;
            Color   color;
            float   textureSlot;
        };
        struct Pipeline
        {
            friend class Renderer2D;
        public:
            Shader::Handle                  hShader             = WZ_NULL_RESOURCE_HANDLE;
            RenderTarget::Handle            hRenderTarget       = WZ_NULL_RESOURCE_HANDLE;
            mat4                            camTransform        = mat4(1.f);
            Color                           clearColor          = Color::blue;
            const size_t                    budget;
            const size_t                    maxObjects          = budget / OBJECT_SIZE;
            const size_t                    maxIndices          = maxObjects * 6; 
            const size_t                    numTextureSlots;

            Pipeline(const size_t& budget = _MB(15));
            ~Pipeline();

            inline const u32& GetIndexCount() const { return indexCount; }
            inline bool IsReady() const { return ready; }

        private:
            typedef std::unordered_map
            <
                Resource::Handle, u32, Resource::Handle::hash
            > TextureIdMap;

            VertexBufferPtr                 vbo                 = NULL;
            VertexArrayPtr                  vao                 = NULL;
            bool                            ready               = false;
            VertexData                      *pBufferHead        = NULL;
            VertexData                      *pBufferCurrent     = NULL;
            u32                             indexCount          = 0;
            u32                             slotCount           = 0;
            TextureIdMap                    textureSlots;
        };

        struct Metrics
        {
            u32 numQuads        = 0;
            u32 numDrawCalls    = 0;
            u32 numIndices      = 0;
            
            u32 GetNumVertices() const 
            { 
                return numQuads * 4; 
            }
            u32 GetMemoryUsage() const
            { 
                return GetNumVertices() * VERTEX_SIZE + numIndices * sizeof(u32); 
            }
        };

    private:
        static constexpr size_t VERTEX_SIZE = (sizeof(VertexData));
        static constexpr size_t OBJECT_SIZE = (VERTEX_SIZE * 4) + sizeof(u32) * 4;
    public:

        static void Init();
        static void Shutdown();

        static void Begin(Pipeline* pipeline);

        static void SubmitTexture(
            Pipeline*               pipeline,
            const Texture::Handle&  hTexture,  
            const vec2&             position, 
            const vec2              scale, 
            float                   rotation, 
            const Color&            color, 
            const Rect&             renderRect = Rect(0, 0, 0, 0)
        );
        static void SubmitTexture(
            Pipeline*               pipeline,
            const Texture::Handle&  hTexture, 
            const mat4&             transform, 
            const Color&            color, 
            const Rect&             renderRect = Rect(0, 0, 0, 0)
        );

        static void SubmitRenderTarget(
            Pipeline*                   pipeline,
            const RenderTarget::Handle& hTexture,  
            const vec2&                 position, 
            const vec2                  scale, 
            float                       rotation, 
            const Color&                color, 
            const Rect&                 renderRect = Rect(0, 0, 0, 0)
        );
        static void SubmitRenderTarget(
            Pipeline*                   pipeline,
            const RenderTarget::Handle& hTexture, 
            const mat4&                 transform, 
            const Color&                color, 
            const Rect&                 renderRect = Rect(0, 0, 0, 0)
        );

        static void SubmitText(
            Pipeline*           pipeline,
            const Font::Handle& hFont,  
            const string&       text,
            const vec2&         position, 
            const vec2          scale, 
            float               rotation, 
            const Color&        color
        );
        static void SubmitText(
            Pipeline*           pipeline,
            const Font::Handle& hFont, 
            const string&       text,
            const mat4&         transform, 
            const Color&        color
        );

        static void SubmitRect(
            Pipeline*       pipeline,
            const Rect&     rect, 
            const Color&    color, 
            RectMode        mode = RectMode::Filled
        );

        static void End(Pipeline* pipeline);

        static void Abort(Pipeline* pipeline);

        static void Clear(Pipeline* pipeline);

        inline static const Shader::Handle& GetFallbackShader() { return s_hFallbackShader; }

        inline static const Metrics& GetMetrics() { return s_metrics; }
        inline static void ResetMetrics() { s_metrics = Metrics(); }

    private:
        static void Submit(
            Pipeline*       pipeline,
            u32             textureSlot,
            vec2&&          size,
            mat4            transform,
            const Color&    color,
            const Rect&     renderRect
        );

    private:
        static Texture::Handle  s_hWhite1x1Texture;
        static Shader::Handle   s_hFallbackShader;
        static Shader::Handle   s_hTextShader;
        static Metrics          s_metrics;
    };
}