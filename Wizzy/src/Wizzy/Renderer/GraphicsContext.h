#pragma once

namespace Wizzy {

    class GraphicsContext {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        virtual void ClearBuffer() = 0;
        virtual void SetClearColor(float r, float g, float b, float a) = 0;
        virtual string APIVersion() = 0;
        virtual string APIVendor() = 0;
    };

}
