#pragma once

namespace Wizzy {

    class GraphicsContext {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        virtual string APIVersion() = 0;
        virtual string APIVendor() = 0;
    };

}
