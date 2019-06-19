#include "wzpch.h"

#include "Wizzy/Resources/Shader.h"
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

    Shader* Shader::Create(const string& file) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::NONE: WZ_CORE_ERROR("No renderer API is selected"); return nullptr;
            case RendererAPI::OPENGL: return new GLShader(file); break;
        }

        WZ_CORE_ASSERT(false, "Invalid renderer API selected");
        return nullptr;
    }
}
