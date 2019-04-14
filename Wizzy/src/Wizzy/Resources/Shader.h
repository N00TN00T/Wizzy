#pragma once

#include "Wizzy/Resources/IResource.h"

namespace Wizzy {
    class Shader
        : public IResource {
    public:
        Shader(string vPath, string fPath);

        void Use();

        void SetUniformMat4(const string& name, const mat4& value);

    private:
        bool        Load();
        bool        __Load_OpenGL();

    private:
        string      m_vPath, m_fPath;
        u32         m_shaderId;

    private:
        static u32 s_currentShader;
    };
}
