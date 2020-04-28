#pragma once

#include "Wizzy/Ecs/Component.h"
#include "Wizzy/Events/Event.h"

namespace Wizzy {
	class System {
	public:
		enum ComponentFlags {
			component_flag_none = 0, component_flag_optional = 1, flag_optional_min_1 = 2
		};
		//virtual void Init(ComponentGroup& components) const {}
		virtual void OnEvent(const Wizzy::Event& e,
							 ComponentGroup& components) const = 0;
		inline const std::vector<StaticCId>& GetTypeIds() const { return m_typeIds; }
		inline const std::vector<ComponentFlags>& GetFlags() const { return m_flags; }
		inline const bool& IsValid() const { return m_isValid; }

		inline bool ProcessAll() const { return m_processAllComponents; }

		bool IsSubscribed(Wizzy::EventType eventId) const;

	protected:
		void AddComponentType(StaticCId type, ComponentFlags flags = component_flag_none);
		template <typename TComponent>
		void AddComponentType(ComponentFlags flags = component_flag_none);
		void AddAllComponentTypes(ComponentFlags flags = component_flag_none);
		void Subscribe(Wizzy::EventType eventId);
		void SubscribeAll();

	private:
		std::vector<StaticCId> m_typeIds;
		std::vector<ComponentFlags> m_flags;
		bool m_isValid = false;
		std::vector<Wizzy::EventType> m_subscriptions;
		bool m_subscribeAll = false;
		bool m_processAllComponents = false;

 	};

	class SystemLayer {
	public:
		template <typename TSystem>
		inline void AddSystem(bool enabled = true) {
			auto _newSystem = new TSystem;
			WZ_CORE_ASSERT(_newSystem->IsValid(), "Invalid system");
			m_systems.push_back(_newSystem);
			m_systemEnabledFlags[_newSystem] = enabled;
		}
		template <typename TSystem>
		inline void RemoveSystem() {
			for (size_t i = 0; i < m_systems.size(); i++) {
				if (typeid(TSystem) == typeid(m_systems[i])) {
					m_systems.erase(m_systems.begin() + i);
					return;
				}
			}
		}

		template <typename TSystem>
		inline void DisableSystem()
		{
			for (int i = 0; i < m_systems.size(); i++)
			{
				if (typestr(TSystem) == typestr(m_systems[i]))
				{
					m_systemEnabledFlags[m_systems[i]] = false;
				}
			}
		}

		template <typename TSystem>
		inline void EnableSystem()
		{
			for (int i = 0; i < m_systems.size(); i++)
			{
				if (typestr(TSystem) == typestr(m_systems[i]))
				{
					m_systemEnabledFlags[m_systems[i]] = true;
				}
			}
		}

		inline bool Enabled(System* s) const 
		{ 
			WZ_CORE_ASSERT(m_systemEnabledFlags.find(s) != m_systemEnabledFlags.end(),
				"System not in system layer");
			return m_systemEnabledFlags.at(s); 
		}

		inline const std::vector<System*> GetSystems() const { return m_systems; }

	private:
		std::vector<System*> m_systems;
		std::unordered_map<System*, bool> m_systemEnabledFlags;
	};

	template<typename TComponent>
	inline void System::AddComponentType(ComponentFlags flags) {
		AddComponentType(TComponent::staticId, flags);
	}
}
