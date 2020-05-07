#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy 
{
    class AudioClip : public Resource
    {
    public:
        __HANDLE_DEF;

        AudioClip(const ResData& data, const PropertyTable& props);
        
        ResData Serialize() const override;

    public:
        static bool IsFileBinary() { return true; }

        static const PropertyTable& GetTemplateProps();

        static Resource* Create(const ResData& data, const PropertyTable& props);

    private:

    };
}