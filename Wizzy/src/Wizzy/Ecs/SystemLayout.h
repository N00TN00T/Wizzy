#pragma once

#include "Wizzy/Ecs/System.h"
#include "Wizzy/Ecs/Ecs.h"
#include "Wizzy/Events/Event.h"

namespace Wizzy 
{
    class SystemLayer
    {
    private:
        typedef std::function<bool(const Event& e, EcsInstance* ecs)> SystemProcessCallback;
    
    public:

        template <typename T>
        inline u32 Push(bool enabled = true)
        {
            void* handle = (void*)T::GetInstance();
            m_systemHandles.push_back(handle);
            m_processFunctions[handle] = [this](const Event& e, EcsInstance* ecs)
            {
                return ecs->ProcessSystem<T>(e);
            };

            m_enabledFlags[handle] = enabled;
            
            if (m_count.find(handle) == m_count.end())
            {
                m_count[handle] = 0;
            }

            m_count[handle]++;

            return m_systemHandles.size() - 1;
        }

        template <typename T>
        inline u32 Insert(u32 index, bool enabled = true)
        {
            void* handle = (void*)T::GetInstance();
            m_systemHandles.insert(m_systemHandles.begin() + index, handle);
            m_processFunctions[handle] = [this](const Event& e, EcsInstance* ecs)
            {
                return ecs->ProcessSystem<T>(e);
            };

            m_enabledFlags[handle] = enabled;
            
            if (m_count.find(handle) == m_count.end())
            {
                m_count[handle] = 0;
            }

            m_count[handle]++;

            return m_systemHandles.size() - 1;
        }

        inline void Pop()
        {
            WZ_CORE_ASSERT(m_systemHandles.size() > 0, "No systems to pop");
            auto handle = m_systemHandles[m_systemHandles.size() - 1];
            m_count[handle]--;
            if (m_count[handle] == 0) m_count.erase(handle);
            m_systemHandles.pop_back();
            m_enabledFlags.erase(handle);
            m_processFunctions.erase(handle);
        }

        template <typename T>
        inline void RemoveFirst()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "System not in layout");
            
            for (int i = 0; i < m_systemHandles.size(); i++)
            {
                if ((void*)T::GetInstance() == m_systemHandles[i])
                {
                    m_systemHandles.erase(m_systemHandles.begin() + i);
                    return;
                }
            }

            WZ_BREAK;
        }

        template <typename T>
        inline void RemoveLast()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "System not in layout");
            
            for (int i = m_systemHandles.size() - 1; i >= 0; i--)
            {
                if ((void)T::GetInstance() == m_systemHandles[i])
                {
                    m_systemHandles.erase(m_systemHandles.begin() + i);
                    return;
                }
            }

            WZ_BREAK;
        }

        inline void Remove(u32 index)
        {
            WZ_CORE_ASSERT(index < m_systemHandles.size(), "System not in layout");
            m_systemHandles.erase(m_systemHandles.begin() + index);
        }

        template <typename T>
        inline void SetEnabled(bool enabled)
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "Cannot SetEnabled on system that's not in layout");
            m_enabledFlags[T::GetInstance()] = enabled;
        }

        inline void SetEnabled(u32 index, bool enabled)
        {
            WZ_CORE_ASSERT(index < m_systemHandles.size(),
                "Layout system index out of range");

            m_enabledFlags[m_systemHandles[index]] = enabled;
        }

        template <typename T>
        inline bool IsEnabled()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "Cannot check IsEnabled for system that's not in layout");
            return m_enabledFlags[T::GetInstance()];
        }

        inline bool IsEnabled(u32 index)
        {
            WZ_CORE_ASSERT(index < m_systemHandles.size(),
                "Cannot check IsEnabled for system that's not in layout");
            return m_enabledFlags[m_systemHandles[index]];
        }

        template <typename T>
        inline bool Has()
        {
            return m_count.find(T::GetInstance()) != m_count.end();
        }

        template <typename T>
        inline bool Is(u32 index)
        {
            return m_systemHandles[index] == (void*)T::GetInstance();
        }

        template <typename T>
        inline u32 Count()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "Cannot check IsEnabled for system that's not in layout");
            return m_count[T::GetInstance()];
        }

        inline u32 Count() const
        {
            return m_systemHandles.size();
        }

        template <typename T>
        inline u32 IndexOfFirst()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "System not in layout");
            
            for (int i = 0; i < m_systemHandles.size(); i++)
            {
                if ((void*)T::GetInstance() == m_systemHandles[i]) return i;
            }

            WZ_BREAK;
            return 0;
        }

        template <typename T>
        inline u32 IndexOfLast()
        {
            WZ_CORE_ASSERT(m_count.find(T::GetInstance()) != m_count.end(),
                "System not in layout");
            
            for (int i = m_systemHandles.size() - 1; i >= 0; i--)
            {
                if ((void*)T::GetInstance() == m_systemHandles[i]) return i;
            }

            WZ_BREAK;
            return 0;
        }

        inline bool _Process(u32 index, const Event& e, EcsInstance* ecs)
        {
            return m_processFunctions[m_systemHandles[index]](e, ecs);
        }

        inline void SetEnabledAll(bool enabled)
        {
            for (auto handle : m_systemHandles)
            {
                m_enabledFlags[handle] = enabled;
            }
        }

    private:
        // TODO: Think of a less spaghetti solution (#RETHINK)
        std::vector<void*> m_systemHandles;

        std::unordered_map<void*, bool> m_enabledFlags;
        std::unordered_map<void*, SystemProcessCallback> m_processFunctions;
        std::unordered_map<void*, u32> m_count;
    };
}