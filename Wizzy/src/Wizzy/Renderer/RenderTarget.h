#pragma once

#include "Wizzy/Renderer/API.h"

namespace Wizzy {

    class RenderTarget {
    public:
        virtual
        ~RenderTarget() {}

        virtual
        void Bind() const = 0;
        virtual
        void Unbind() const = 0;

        virtual
        u32 GetTextureId() const = 0;
        virtual
        u32 GetWidth() const = 0;
        virtual
        u32 GetHeight() const = 0;

        virtual
        void SetSize(u32 width, u32 height) = 0;


        static
        RenderTarget* Create(u32 width, u32 height);
    };

    typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
}
