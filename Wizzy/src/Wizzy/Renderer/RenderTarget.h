#pragma once

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/Texture.h"

namespace Wizzy {

    

    class RenderTarget : public Resource {
    public:
        __HANDLE_DEF;
        RenderTarget(const PropertyTable& props) : Resource(props) {}

        virtual
        ~RenderTarget() {}

        virtual
        void Bind() const = 0;
        virtual
        void Unbind() const = 0;

        virtual void BindTexture(u32 location) const = 0;
        virtual void UnbindTexture() const = 0;

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

        static Resource* Create(const ResData& data, const PropertyTable& props);

        static const PropertyTable& GetTemplateProps();

        inline static bool IsFileBinary() { return true; }
    };

    typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
}
