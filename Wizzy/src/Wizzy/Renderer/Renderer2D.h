#pragma once

#include "Wizzy/Math/Rect.h"
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
            fvec2    pos;
            fvec2    uv;
            color4   color;
            f32      textureSlot;
        };
        struct Pipeline
        {
            friend class Renderer2D;
        public:
            Shader::Handle                  hShader             = WZ_NULL_RESOURCE_HANDLE;
            RenderTarget::Handle            hRenderTarget       = WZ_NULL_RESOURCE_HANDLE;
            fmat4                           camTransform        = projection::ortho<float>(0, 1600, 0, 900, -1, 1);
            color3                          clearColor          = COLOR_BLUE;
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
            typedef std::unordered_map
            <
                Font::Handle,
                u32,
                Font::Handle::hash
            > FontBudgetOverflowMap;

            VertexBufferPtr                 vbo                     = NULL;
            VertexArrayPtr                  vao                     = NULL;
            bool                            ready                   = false;
            VertexData                      *pBufferHead            = NULL;
            VertexData                      *pBufferCurrent         = NULL;
            u32                             indexCount              = 0;
            u32                             slotCount               = 0;
            TextureIdMap                    textureSlots;
            FontBudgetOverflowMap           lastFontBudgetOverflows;
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

    public:
        static constexpr size_t VERTEX_SIZE = (sizeof(VertexData));
        static constexpr size_t OBJECT_SIZE = (VERTEX_SIZE * 4) + sizeof(u32) * 4;
    public:

        static void Init();
        static void Shutdown();

        static void Begin(Pipeline* pipeline);

        static void SubmitTexture(
            Pipeline*               pipeline,
            const Texture::Handle&  hTexture,  
            const fvec2&            position, 
            const fvec2&            scale = fvec2(1.f), 
            float                   rotation = 0.f, 
            const color&            color = COLOR_WHITE, 
            const Rect&             renderRect = Rect(0, 0, 0, 0)
        );
        static void SubmitTexture(
            Pipeline*               pipeline,
            const Texture::Handle&  hTexture, 
            const fmat4&            transform, 
            const color&            color = COLOR_WHITE, 
            const Rect&             renderRect = Rect(0, 0, 0, 0)
        );

        static void SubmitRenderTarget(
            Pipeline*                   pipeline,
            const RenderTarget::Handle& hTexture,  
            const fvec2&                position, 
            const fvec2&                scale = fvec2(1.f), 
            float                       rotation = 0.f, 
            const color&                color = COLOR_WHITE, 
            const Rect&                 renderRect = Rect(0, 0, 0, 0)
        );
        static void SubmitRenderTarget(
            Pipeline*                   pipeline,
            const RenderTarget::Handle& hTexture, 
            const fmat4&                transform, 
            const color&                color = COLOR_WHITE, 
            const Rect&                 renderRect = Rect(0, 0, 0, 0)
        );

        static void SubmitText(
            Pipeline*           pipeline,
            const Font::Handle& hFont,  
            const string&       text,
            const fvec2&        position, 
            const fvec2&        scale = fvec2(1.f), 
            f32                 rotation = 0.f, 
            const color&        color = COLOR_WHITE
        );
        static void SubmitText(
            Pipeline*           pipeline,
            const Font::Handle& hFont, 
            const string&       text,
            const fmat4&        transform, 
            const color&        color = COLOR_WHITE
        );

        static void SubmitRect(
            Pipeline*       pipeline,
            const Rect&     rect, 
            const color&    color = COLOR_WHITE, 
            RectMode        mode = RectMode::Filled
        );

        static void SubmitRect(
            Pipeline*       pipeline,
            const Rect&     rect, 
            f32             rotation = 0.f,
            const color&    color = COLOR_WHITE, 
            RectMode        mode = RectMode::Filled
        );

        static void SubmitLine(
            Pipeline*       pipeline,
            fvec2           a,
            fvec2           b,
            const color&    color = COLOR_WHITE
        );

        static void End(Pipeline* pipeline);

        static void Abort(Pipeline* pipeline);

        static void Clear(Pipeline* pipeline);

        inline static const Shader::Handle& GetFallbackShader() { return s_hFallbackShader; }
        inline static const Shader::Handle& GetTextShader() { return s_hTextShader; }

        inline static const Metrics& GetMetrics() { return s_metrics; }
        inline static void ResetMetrics() { s_metrics = Metrics(); }

        inline static const Texture::Handle& GetWhite1x1Texture() { return s_hWhite1x1Texture; }

    private:
        static void Submit(
            Pipeline*       pipeline,
            u32             textureSlot,
            fvec2&&         size,
            fmat4           transform,
            const color&    color,
            const Rect&     renderRect
        );

    private:
        static Texture::Handle  s_hWhite1x1Texture;
        static Shader::Handle   s_hFallbackShader;
        static Shader::Handle   s_hTextShader;
        static Metrics          s_metrics;
    };

    typedef Renderer2D::Pipeline RenderPipeline2D;
}