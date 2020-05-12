#include "wzpch.h"

#ifdef WZ_COMPILE_EXTENSIONS || defined(WZ_COMPILE_EXTENSION_GAME)

#include "Wizzy/Extensions/Game/Common.h"


WZ_NS_BEGIN(Wizzy);
WZ_NS_BEGIN(Game);

ResourceSystem::ResourceSystem()
{
    Subscribe(EventType::app_init);
    Subscribe(EventType::app_frame_begin);
    Subscribe(EventType::app_update);
    Subscribe(EventType::resource_load);
    Subscribe(EventType::resource_unload);
    Subscribe(EventType::resource_file_change);
}

bool ResourceSystem::ProcessComponents(const Event& e, ResourceConfig* config) const
{
    switch (e.GetEventType())
    {
        case EventType::app_init:
        {
            if (ResourceManagement::GetResourceDir() != config->resDir)
                ResourceManagement::SetResourceDir(config->resDir);
            break;
        }
        case EventType::app_frame_begin:
        {
            if (ResourceManagement::GetResourceDir() != config->resDir)
                ResourceManagement::SetResourceDir(config->resDir);

            ResourceManagement::Validate(true);

            break;
        }
        case EventType::app_update:
        {
            
            break;
        }
        case EventType::resource_load:
        {
            auto& loadEvent = (const ResourceLoadedEvent&)e;

            
            break;
        }
        case EventType::resource_unload:
        {
            auto& unloadEvent = (const ResourceUnloadedEvent&)e;

            
            break;
        }
        case EventType::resource_file_change:
        {
            break;
        }
    }

    return false;
}

WZ_NS_END;
WZ_NS_END; 

#endif