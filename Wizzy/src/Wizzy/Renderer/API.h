#pragma once

namespace Wizzy {

    struct Viewport {
        u32 x, y, width, height;

        Viewport(u32 x, u32 y, u32 width, u32 height)
            : x(x), y(y), width(width), height(height) {}
    };

    enum class ShaderDataType : int8 {
        NONE = 0,
        FLOAT1, FLOAT2, FLOAT3, FLOAT4,
        MAT3, MAT4,
        INT1, INT2, INT3, INT4,
        BOOL
    };

    enum CullMode : int8 {
        WZ_CULL_NONE = 0,
        WZ_CULL_FRONT,
        WZ_CULL_BACK
    };

    enum TextureEnum : int8 {

        WZ_NONE = 0,

        WZ_MIN_FILTER_MODE_NEAREST,
        WZ_MIN_FILTER_MODE_LINEAR,
        WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_NEAREST,
        WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_NEAREST,
        WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_LINEAR,
        WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR,

        WZ_MAG_FILTER_NEAREST,
        WZ_MAG_FILTER_LINEAR,

        WZ_WRAP_MODE_CLAMP_TO_EDGE,
        WZ_WRAP_MODE_MIRRORED_REPEAT,
        WZ_WRAP_MODE_REPEAT,

        WZ_DONT_GENERATE_MIPMAP,
    };

    enum ShaderEnum : int8 {
        WZ_SHADER_LIGHTING_MODE_NONE = 0,
        WZ_SHADER_LIGHTING_MODE_PHONG,
        WZ_SHADER_LIGHTING_MODE_PBR
    };

    enum RenderMode : int8 {
        WZ_RENDER_MODE_NONE = 0,
        WZ_RENDER_MODE_LINES,
        WZ_RENDER_MODE_TRIANGLES
    };

    int32 ShaderDataTypeToAPIType(ShaderDataType sType);
    int32 CullModeToAPICullMode(CullMode mode);
    int32 RenderModeToAPIRenderMode(RenderMode mode);
    int32 MaxTextureSlot();

    string APIVersion();

    string APIVendor();
}
