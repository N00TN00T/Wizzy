#pragma once

#include "Wizzy/Ecs/Component.h"
#include "Wizzy/Events/Event.h"
#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/Utilities/TemplateUtils.h"

namespace Wizzy {

	typedef Bitset SystemSignature;



	template <typename TSubclass, typename ...TComponents>
	class System 
	{
	public:
		enum ComponentFlags
		{
			flag_none = 0,
			flag_optional = BIT(1),
		};
		typedef System<TSubclass, TComponents...> SystemImpl;
		

		static constexpr u32 NUM_COMPONENT_TYPES =  sizeof...(TComponents);
		static const ComponentId COMPONENT_TYPES[NUM_COMPONENT_TYPES];

	public:
		typedef std::array<IComponent*, NUM_COMPONENT_TYPES> ComponentPass;

		inline bool ProcessComponentsPass(const Event& e, ComponentPass& components) const
		{
			return ProcessComponents(e, ((TComponents*)components.at(index_of_type<TComponents, TComponents...>()))...);
		}

		inline bool IsSubscribed(EventType eventType) const 
		{ 
			return m_subscriptions.Get((s32)eventType);
		}

		inline bool HasType(ComponentId type) 
		{
			return fullSignature().Get(type);
		}

		inline const SystemSignature& GetFullSignature() const
		{
			return fullSignature();
		}

		inline const SystemSignature& GetMinimumSignature() const
		{
			return m_minimumSignature;
		}

		inline bool HasTypeFlag(ComponentId type, s32 flag) 
		{
			WZ_CORE_ASSERT(HasType(type), 
				"Cannot check type flag for component that is not in system signature");
			if (m_flags.find(type) == m_flags.end())
				m_flags[type] = flag_none;
			return m_flags.at(type) & flag;
		}

		template <typename TComponent>
		inline void Flag(s32 flags)
		{
			assert(HasType(TComponent::typeId()) && "System must have type to flag it");
			if (m_flags.find(TComponent::typeId()) == m_flags.end()) 
				m_flags[TComponent::typeId()] = flag_none;
			m_flags[TComponent::typeId()] |= flags;
			if (flags |= flag_optional)
			{
				m_minimumSignature.Set(TComponent::typeId(), false);
			}
		}

	protected:
		inline void Subscribe(EventType eventType)
		{
			m_subscriptions.Set((s32)eventType, true);
		}

		virtual bool ProcessComponents(const Event& e, TComponents*...) const = 0;

	private:
		std::unordered_map<ComponentId, s32> m_flags;
		Bitset m_subscriptions;
		SystemSignature m_minimumSignature = fullSignature();

	public:
		inline static TSubclass* GetInstance() { return &s_instance; }
		inline static ComponentPass& GetComponentPass() { return s_componentPass; }
		inline static u32 GetComponentIndex(ComponentId type) {  return s_componentIndices.at(type);};

	protected:
		static TSubclass s_instance;
	private:
		template <typename ...TComponents>
		inline static SystemSignature CreateFullSignature()
		{
			SystemSignature sig;


			(sig.Set(TComponents::typeId(), true), ...);

			return sig;
		}

		inline static const SystemSignature& fullSignature()
		{
			static SystemSignature s_fullSignature = CreateFullSignature<TComponents...>();
			return s_fullSignature;
		}

		static ComponentPass s_componentPass;
		static const std::unordered_map<ComponentId, u32> s_componentIndices;
 	};

	#define _SystemImpl System<TSubclass, TComponents...>

	template <typename ...TComponent>
	std::unordered_map<ComponentId, u32> BuildIndexMap()
	{
		u32 idx = 0;

		std::unordered_map<ComponentId, u32> map;
		(map.emplace(TComponent::typeId(), idx++), ...);

		return map;
	}

	template <typename TSubclass, typename ...TComponents>
	TSubclass _SystemImpl::s_instance = TSubclass();

	template <typename TSubclass, typename ...TComponents>
	const ComponentId _SystemImpl::COMPONENT_TYPES[_SystemImpl::NUM_COMPONENT_TYPES] =
	{
		TComponents::typeId()...
	};

	template <typename TSubclass, typename ...TComponents>
	std::array<IComponent*, _SystemImpl::NUM_COMPONENT_TYPES> _SystemImpl::s_componentPass;

	template <typename TSubclass, typename ...TComponents>
	const std::unordered_map<ComponentId, u32> _SystemImpl::s_componentIndices = BuildIndexMap<TComponents...>();
}
