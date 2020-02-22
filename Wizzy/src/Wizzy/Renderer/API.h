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
        BOOL,
		TEXTURE2D
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

    /*
        STREAM

    The data store contents will be modified once and used at most a few times.
    STATIC

    The data store contents will be modified once and used many times.
    DYNAMIC

    The data store contents will be modified repeatedly and used many times.

    The nature of access may be one of these:

    DRAW

    The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
    READ

    The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
    COPY

    The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
    */
    enum BufferEnum : int8
    {
        //GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
        WZ_BUFFER_USAGE_STREAM_DRAW,
        WZ_BUFFER_USAGE_STREAM_READ,
        WZ_BUFFER_USAGE_STREAM_COPY,
        WZ_BUFFER_USAGE_STATIC_DRAW,
        WZ_BUFFER_USAGE_STATIC_READ,
        WZ_BUFFER_USAGE_STATIC_COPY,
        WZ_BUFFER_USAGE_DYNAMIC_DRAW,
        WZ_BUFFER_USAGE_DYNAMIC_READ,
        WZ_BUFFER_USAGE_DYNAMIC_COPY,
    };

    u32 BufferUsageToAPIBufferUsage(BufferEnum v);

	size_t ShaderDataTypeSize(ShaderDataType type);

	inline 
	string ShaderDataTypeString(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::NONE:			return "NONE";
		case ShaderDataType::FLOAT1:		return "FLOAT1";
		case ShaderDataType::FLOAT2:		return "FLOAT2";
		case ShaderDataType::FLOAT3:		return "FLOAT3";
		case ShaderDataType::FLOAT4:		return "FLOAT4";
		case ShaderDataType::MAT3:			return "MAT3";
		case ShaderDataType::MAT4:			return "MAT4";
		case ShaderDataType::INT1:			return "INT1";
		case ShaderDataType::INT2:			return "INT2";
		case ShaderDataType::INT3:			return "INT3";
		case ShaderDataType::INT4:			return "INT4";
		case ShaderDataType::TEXTURE2D:		return "TEXTURE2D";
		case ShaderDataType::BOOL:			return "BOOL";
		}

		WZ_CORE_ASSERT(false, "Unimplemented shader data type");

		return "";
	}

	inline
	ShaderDataType StringToShaderDataType(const string& str) {
		if (str == "NONE")				return ShaderDataType::NONE;
		else if (str == "FLOAT1")		return ShaderDataType::FLOAT1;
		else if (str == "FLOAT2")		return ShaderDataType::FLOAT2;
		else if (str == "FLOAT3")		return ShaderDataType::FLOAT3;
		else if (str == "FLOAT4")		return ShaderDataType::FLOAT4;
		else if (str == "MAT3")			return ShaderDataType::MAT3;
		else if (str == "MAT4")			return ShaderDataType::MAT4;
		else if (str == "INT1")			return ShaderDataType::INT1;
		else if (str == "INT2")			return ShaderDataType::INT2;
		else if (str == "INT3")			return ShaderDataType::INT3;
		else if (str == "INT4")			return ShaderDataType::INT4;
		else if (str == "TEXTURE2D")	return ShaderDataType::TEXTURE2D;
		else if (str == "BOOL")			return ShaderDataType::BOOL;

		WZ_CORE_ASSERT(false, "'" + str + "' could not be converted to a ShaderDataType");
	}

	u32 ShaderDataTypeElementCount(ShaderDataType type);

    int32 ShaderDataTypeToAPIType(ShaderDataType sType);
    int32 CullModeToAPICullMode(CullMode mode);
    int32 RenderModeToAPIRenderMode(RenderMode mode);
    int32 MaxTextureSlot();

    string APIVersion();

    string APIVendor();

}
