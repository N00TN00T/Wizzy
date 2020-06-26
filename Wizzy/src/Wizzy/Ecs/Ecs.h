#pragma once

#include "Wizzy/Ecs/System.h"
#include "Wizzy/Events/Event.h"
#include "Wizzy/JobSystem/JobSystem.h"

namespace Wizzy
{
	class SystemLayer;

	class EcsInstance
	{
	private:
		struct Entity;

		typedef std::vector<byte> 							DataPool;
		typedef u64											DataIndex;
		typedef u32											EntityId;
		typedef std::map<EntityId, Entity>					EntityMap;
		typedef std::unordered_map<ComponentId, DataPool> 	DataPoolMap;
		
		struct Entity
		{
			std::unordered_map<ComponentId, DataIndex> componentRefs;
			EntityId id;
			SystemSignature signature;
		};

	public:
		template <typename ...T>
		EntityHandle CreateEntity();
		EntityHandle CreateEntity();

		void DestroyEntity(EntityHandle hEntity);

		template <typename T>
		void CreateComponent(EntityHandle hEntity);
		void CreateComponent(EntityHandle hEntity, ComponentId type);

		template <typename T>
		void DestroyComponent(EntityHandle hEntity);
		void DestroyComponent(EntityHandle hEntity, ComponentId type);

		template <typename T>
		T* GetComponent(EntityHandle hEntity);
		IComponent* GetComponent(EntityHandle hEntity, ComponentId type);

		template <typename T>
		bool HasComponent(EntityHandle hEntity);
		bool HasComponent(EntityHandle hEntity, ComponentId type);

		template <typename TSystem>
		bool ProcessSystem(const Event& e);

		bool ProcessLayout(SystemLayer& layout, const Event& e);

		void ForEachEntity(std::function<bool(EntityHandle e)>);

	private:
		Entity& FromHandle(EntityHandle hEntity);
		EntityHandle ToHandle(Entity& entity);

		inline u32 GetComponentCount(ComponentId type)
		{
			auto& typeInfo = IComponent::StaticInfo(type);

			return m_dataPools[type].size() / typeInfo.size;
		}

		inline EntityId GetNextEntityId()
		{
			static EntityId current = 0;
			return ++current;
		}

	private:
		DataPoolMap m_dataPools;
		EntityMap 	m_entities;
	};

	template <typename ...TComponents>
	inline EntityHandle EcsInstance::CreateEntity()
	{
		auto newId = GetNextEntityId();
		auto& entity = m_entities[newId];
		entity.id = newId;

		EntityHandle hEntity = ToHandle(entity);
		(CreateComponent<TComponents>(hEntity), ...);

		return hEntity;
	}

	inline EntityHandle EcsInstance::CreateEntity()
	{
		auto newId = GetNextEntityId();
		auto& entity = m_entities[newId];
		entity.id = newId;
		return ToHandle(entity);
	}

	inline void EcsInstance::DestroyEntity(EntityHandle hEntity)
	{
		auto& entity = FromHandle(hEntity);

		// Copy refs, as its modified in DestroyComponent
		auto componentRefs = entity.componentRefs;
		for (auto [type, dataIndex] : componentRefs)
		{
			DestroyComponent(hEntity, type);
		}

		m_entities.erase(entity.id);
	}

	template <typename T>
	inline void EcsInstance::CreateComponent(EntityHandle hEntity)
	{
		CreateComponent(hEntity, T::typeId());
	}
	inline void EcsInstance::CreateComponent(EntityHandle hEntity, ComponentId type)
	{
		WZ_CORE_ASSERT(IComponent::IsTypeValid(type), 
			"Tried creating a component on entity with invalid ID");
		auto& entity = FromHandle(hEntity);
		WZ_CORE_ASSERT(entity.signature.Get(type) == false, 
			"Tried creating duplicate of Component type on entity. Only one component per type allowed");

		const auto& typeInfo = IComponent::StaticInfo(type);
		const auto& createFn = typeInfo.createFn;
		auto& dataPool = m_dataPools[type];

		auto oldSize = dataPool.size();

		entity.componentRefs[type] = createFn(dataPool, hEntity);
		entity.signature.Set(type, true);

		GetComponent(hEntity, type)->entity = hEntity;
	}

	template <typename T>
	inline void EcsInstance::DestroyComponent(EntityHandle hEntity)
	{
		DestroyComponent(hEntity, T::typeId());
	}
	inline void EcsInstance::DestroyComponent(EntityHandle hEntity, ComponentId type)
	{
		auto& entity = FromHandle(hEntity);
		WZ_CORE_ASSERT(entity.signature.Get(type) == true, 
			"Tried destroying a component the entity doesn't have");

		const auto& typeInfo = IComponent::StaticInfo(type);
		const auto& freeFn = typeInfo.freeFn;
		auto& dataPool = m_dataPools[type];

		IComponent* component = GetComponent(hEntity, type);

		DataIndex deadCompIndex = entity.componentRefs[type];
		DataIndex lastCompIndex = dataPool.size() - typeInfo.size;

		freeFn(component);
		entity.componentRefs.erase(type);
		entity.signature.Set(type, false);

		if (deadCompIndex != lastCompIndex)
		{
			// Move last component to replace dead component
			memcpy(&dataPool[deadCompIndex], &dataPool[lastCompIndex], typeInfo.size);

			IComponent* movedComponent = (IComponent*)&dataPool[deadCompIndex];

			Entity& affectedEntity = FromHandle(movedComponent->entity);
			affectedEntity.componentRefs[type] = deadCompIndex;
		}

		dataPool.resize(dataPool.size() - typeInfo.size);
		// TODO: Maybe shrink_to_fit here to actually deallocate the memory? 
		// Not doing so will avoid dynamic allocations for future components...	
	}

	template <typename T>
	inline T* EcsInstance::GetComponent(EntityHandle hEntity)
	{
		return (T*)GetComponent(hEntity, T::typeId());
	}
	inline IComponent* EcsInstance::GetComponent(EntityHandle hEntity, ComponentId type)
	{
		WZ_CORE_ASSERT(IComponent::IsTypeValid(type), "Invalid component type");
		auto& entity = FromHandle(hEntity);
		WZ_CORE_ASSERT(entity.signature.Get(type) == true, 
			"Tried getting non-existent component from entity");
		auto& dataPool = m_dataPools[type];
		return (IComponent*)&dataPool[entity.componentRefs[type]];
	}

	template <typename T>
	inline bool EcsInstance::HasComponent(EntityHandle hEntity)
	{
		return HasComponent(hEntity, T::typeId());
	}
	inline bool EcsInstance::HasComponent(EntityHandle hEntity, ComponentId type)
	{
		auto& entity = FromHandle(hEntity);

		return entity.signature.Get(type);
	}

	template <typename TSystem>
	inline bool EcsInstance::ProcessSystem(const Event& e)
	{
		bool handled = false;
		auto system = TSystem::GetInstance();
		if (!system->IsSubscribed(e.GetEventType())) return false;
		auto& pass = system->GetComponentPass();

		u32 smallestPoolType = 0;

		// TODO: Maybe there's a way to cache this? (#OPTIMIZE)
		for (int i = 0; i < TSystem::NUM_COMPONENT_TYPES; i++)
		{
			ComponentId type = TSystem::COMPONENT_TYPES[i];

			if (smallestPoolType == 0
				|| (GetComponentCount(type) < GetComponentCount(smallestPoolType)
					&& !system->HasTypeFlag(type, TSystem::flag_optional)))
			{
				smallestPoolType = type;
			}
		}

		if (m_dataPools[smallestPoolType].size() == 0) return false;

		auto& smallestTypeInfo = IComponent::StaticInfo(smallestPoolType);
		auto& smallestPool = m_dataPools[smallestPoolType];

		for (DataIndex dataIndex = 0; dataIndex < smallestPool.size(); dataIndex += smallestTypeInfo.size)
		{
			IComponent* smallestPoolComponent = (IComponent*)&smallestPool[dataIndex];
			Entity& entity = FromHandle(smallestPoolComponent->entity);
			auto& systemSig = system->GetMinimumSignature();
			if ((entity.signature & systemSig) != systemSig) continue;

			// TODO: Generate this code with templates voodoo to avoid for loop? (#OPTIMIZE)
			for (u32 i = 0; i < TSystem::NUM_COMPONENT_TYPES; i++)
			{
				ComponentId type = TSystem::COMPONENT_TYPES[i];

				if (entity.signature.Get(type))
				{
					auto& dataPool = m_dataPools[type];
					auto& dataIndex = entity.componentRefs[type];
					pass[system->GetComponentIndex(type)] = (IComponent*)&dataPool[dataIndex];
				}
				else
				{
					pass[system->GetComponentIndex(type)] = NULL;
				}
				
			}

			if (system->ProcessComponentsPass(e, pass))
			{
				handled = true;
				break;
			}
		}

		return handled;
	}

	inline EcsInstance::Entity& EcsInstance::FromHandle(EntityHandle hEntity)
	{

		WZ_CORE_ASSERT(m_entities.find(reinterpret_cast<EntityId>(hEntity)) 
						!= m_entities.end(), "Invalid Entity Handle");
		return m_entities[reinterpret_cast<EntityId>(hEntity)];
	}
	inline EntityHandle EcsInstance::ToHandle(Entity& entity)
	{
		return (void*)entity.id;
	}

	inline void EcsInstance::ForEachEntity(std::function<bool(EntityHandle e)> fn)
	{
		for (auto& [entityId, entity] : m_entities)
		{
			auto hEntity = ToHandle(entity);

			if (!fn(hEntity)) break;
		}
	}
}