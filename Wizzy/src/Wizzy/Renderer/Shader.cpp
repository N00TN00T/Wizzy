#include "wzpch.h"

#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Utils.h"
#include "Wizzy/platform/OpenGL/GLShader.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {

    const PropertyTable* Shader::s_templateProps;

    Shader::~Shader() {
        
    }

    void Shader::UploadData(const string& name, ShaderDataType type, void* data) {
        switch (type) {
            case ShaderDataType::NONE: WZ_CORE_WARN("Tried uploading shader data with no type"); break;
            case ShaderDataType::FLOAT1:
                Upload1f(name, *static_cast<float*>(data)); break;
            case ShaderDataType::FLOAT2:
                Upload2f(name, *static_cast<vec2*>(data)); break;
            case ShaderDataType::FLOAT3:
                Upload3f(name, *static_cast<vec3*>(data)); break;
            case ShaderDataType::FLOAT4:
                Upload4f(name, *static_cast<vec4*>(data)); break;
            case ShaderDataType::MAT3:
                UploadMat3(name, *static_cast<mat3*>(data)); break;
            case ShaderDataType::MAT4:
                UploadMat4(name, *static_cast<mat4*>(data)); break;
            case ShaderDataType::INT1:
                Upload1i(name, *static_cast<int*>(data)); break;
            case ShaderDataType::INT2:
                Upload2i(name, static_cast<int*>(data)); break;
            case ShaderDataType::INT3:
                Upload3i(name, static_cast<int*>(data)); break;
            case ShaderDataType::INT4:
                Upload4i(name, static_cast<int*>(data)); break;
            case ShaderDataType::BOOL:
                Upload1i(name, *static_cast<int*>(data)); break;
            default: WZ_CORE_ASSERT(false, "GL Unimplemented shader data type"); break;
        }
    }

    Resource* Shader::Create(const ResData& data, const PropertyTable& props) {

        CREATE_BY_API(new GLShader(data, props));

        return nullptr;
    }
    const PropertyTable& Shader::GetTemplateProps()
    {
        if (!s_templateProps)
        {
            s_templateProps = new PropertyTable;
        }
        return *s_templateProps;
    }
}
