#include "wzpch.h"

#include "Wizzy/Audio/AudioClip.h"

namespace Wizzy
{
    AudioClip::AudioClip(const ResData& data, const PropertyTable& props)
        : Resource(props) 
    {

    }
        
    ResData AudioClip::Serialize() const
    {
        return ResData();
    }

    const PropertyTable& AudioClip::GetTemplateProps()
    {
        static PropertyTable p;
        return p;
    }

    Resource* AudioClip::Create(const ResData& data, const PropertyTable& props)
    {
        return new AudioClip(data, props);
    }
}
