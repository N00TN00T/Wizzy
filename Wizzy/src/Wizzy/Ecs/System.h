#pragma once

#include "Wizzy/Ecs/Component.h"
#include "Wizzy/Events/Event.h"

namespace Wizzy {
	class System {
	public:
		enum ComponentFlags {
			FLAG_NONE = 0, FLAG_OPTIONAL = 1, FLAG_OPTIONAL_MIN_1 = 2
		};
		virtual void OnEvent(const Wizzy::Event& e,
							 ComponentGroup& components) const = 0;
		inline const std::vector<StaticCId>& GetTypeIds() const { return m_typeIds; }
		inline const std::vector<ComponentFlags>& GetFlags() const { return m_flags; }
		inline const bool& IsValid() const { return m_isValid; }

		inline bool ProcessAll() const { return m_processAllComponents; }

		bool IsSubscribed(Wizzy::EventType eventId) const;

	protected:
		void AddComponentType(StaticCId type, ComponentFlags flags = FLAG_NONE);
		template <typename TComponent>
		void AddComponentType(ComponentFlags flags = FLAG_NONE);
		void AddAllComponentTypes(ComponentFlags flags = FLAG_NONE);
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
		inline void AddSystem() {
			auto _newSystem = new TSystem;
			WZ_CORE_ASSERT(_newSystem->IsValid(), "Invalid system");
			m_systems.push_back(_newSystem);
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

		inline size_t SystemCount() const { return m_systems.size(); }

		inline const System* operator[](size_t index) const {
			WZ_CORE_ASSERT(index >= 0 && index < m_systems.size(), "System layer index out of range");
			return m_systems.at(index);
		}
	private:
		std::vector<System*> m_systems;
	};

	template<typename TComponent>
	inline void System::AddComponentType(ComponentFlags flags) {
		AddComponentType(TComponent::staticId, flags);
	}
}
