#pragma once

#include "Wizzy/Ecs/System.h"
#include "Wizzy/PropertyLibrary.h"

namespace Wizzy
{

	/*
		Used for remembering the index in the component
		memory pool where the data of the components
		on an entity begins.
	*/
	typedef u64 MemIndex;

	/*
		Used for referencing in the mem pool which
		components belong to an entity
	*/
	typedef std::vector<std::pair<StaticCId, MemIndex>> EntityDataRef;

	/*
		Entities are only a collection of components,
		or the indices of the components in the ECS
		component collection.
	*/
	typedef std::pair<u64, EntityDataRef> Entity;


	class ECSManager
	{
	public:
		ECSManager();
		ECSManager(const ECSManager&) = delete;
		~ECSManager();

		template <typename ...TComponent>
		EntityHandle CreateEntity(TComponent&... args)
		{
			Wizzy::IComponent* comps[] =
			{
				&args...
			};

			Wizzy::StaticCId ids[] =
			{
				args.staticId...
			};

			return this->CreateEntity(comps, ids, sizeof(comps) / sizeof(Wizzy::IComponent*));
		}

		template <typename ...TComponent>
		EntityHandle CreateEntity()
		{
			Wizzy::IComponent* comps[] =
			{
				new TComponent...
			};

			Wizzy::StaticCId ids[] =
			{
				TComponent::staticId...
			};

			auto hnd = this->CreateEntity(comps, ids, sizeof(comps) / sizeof(Wizzy::IComponent*));
			for (int i = 0; i < sizeof(comps) / sizeof(Wizzy::IComponent*); i++)
			{
				delete comps[i];
			}
			return hnd;
		}

		EntityHandle CreateEntity(IComponent** components,
			const StaticCId* ids,
			size_t numComponents);
		void RemoveEntity(EntityHandle handle);

		template <typename TComponent>
		void AddComponent(EntityHandle entity, TComponent* component);
		template <typename TComponent>
		void RemoveComponent(EntityHandle entity);
		template <typename TComponent>
		TComponent* GetComponent(EntityHandle entity) const;

		std::vector<std::pair<StaticCId, IComponent*>> GetComponents(EntityHandle entity) const;

		void NotifySystems(const SystemLayer& systems, const Wizzy::Event& e) const;

		PropertyTable Serialize() const;
		void Deserialize(const PropertyTable& table);

		void Clear();

	private:
		std::unordered_map<StaticCId, ComponentMem>		m_components;
		std::vector<Entity*>							m_entities;
		mutable std::mutex								m_updateMutex;

		const string TOKEN_ENTITY = "ENTITY";
		const string TOKEN_COMPONENT = "COMPONENT";

		inline Entity* ToRawType(EntityHandle handle) const
		{
			return static_cast<Entity*>(handle);
		}

		inline const MemIndex& ToEntityMemIndex(EntityHandle handle) const
		{
			return ToRawType(handle)->first;
		}

		inline EntityDataRef& ToEntityData(EntityHandle handle) const
		{
			return ToRawType(handle)->second;
		}

		void AddComponentInternal(Entity* entity,
			const StaticCId& componentId,
			IComponent* component);
		void RemoveComponentInternal(EntityHandle handle,
			const StaticCId componentId);
		IComponent* GetComponentInternal(Entity* entity,
			const ComponentMem& memPool,
			const StaticCId& componentId) const;
		void DeleteComponent(const StaticCId& componentId,
			const MemIndex& memIdx);

		uint32_t FindLeastCommonComponentIndex(const std::vector<StaticCId>& types,
			const std::vector<System::ComponentFlags>& flags) const;


	};

	template<typename TComponent>
	inline void ECSManager::AddComponent(EntityHandle entity, TComponent* component)
	{
		AddComponentInternal(ToRawType(entity), TComponent::staticId, component);
	}
	template<typename TComponent>
	inline void ECSManager::RemoveComponent(EntityHandle entity)
	{
		RemoveComponentInternal(entity, TComponent::staticId);
	}
	template<typename TComponent>
	inline TComponent* ECSManager::GetComponent(EntityHandle entity) const
	{
		return static_cast<TComponent*>(GetComponentInternal(ToRawType(entity),
			m_components.at(TComponent::staticId),
			TComponent::staticId));
	}
}
