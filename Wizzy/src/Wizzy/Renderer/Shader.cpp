#include "wzpch.h"

#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Utils.h"
#include "Wizzy/platform/OpenGL/OpenGLShader.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {

    Shader::~Shader() {

    }

    void Shader::Load() {
        ParseShader(this->GetSourceFile());
        WZ_CORE_ASSERT(this->Compile(), "Failed compiling shader source");
    }
    void Shader::Unload() {
        WZ_CORE_TRACE("Unloading shader...");
        WZ_CORE_ASSERT(!this->IsGarbage(), "Tried unloading a garbage-flagged Shader");
        this->Delete();
    }
    void Shader::Reload() {
        WZ_CORE_TRACE("Reloading shader...");
        Unload();
        Load();
    }
    void Shader::Save() {

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

    Shader* Shader::Create(const string& file) {

        CREATE_BY_API(new GLShader(file));

        return nullptr;
    }
}
